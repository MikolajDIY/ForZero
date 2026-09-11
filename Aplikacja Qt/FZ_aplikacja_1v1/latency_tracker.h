#ifndef LATENCY_TRACKER_H
#define LATENCY_TRACKER_H

#include <chrono>
#include <unordered_map>
#include <vector>
#include <numeric>
#include <algorithm>
#include <limits>
#include <utils.h>

using Clock = std::chrono::high_resolution_clock;
using TimePoint = Clock::time_point;

struct PendingFrame{
    TimePoint sentAt{};
    // bool pending = false;
    bool collided = false;
};

class LatencyTracker : public QObject {
public:


    // 20 000 próbek = ok. 3.5 minuty przy 100 Hz / ok. 5.5 minuty przy 60 Hz
    explicit LatencyTracker(size_t maxSamples = 20000)
        : m_maxSamples(maxSamples) {
        m_samples.reserve(maxSamples);
    }

    void markSent(uint16_t seqId, TimePoint udpTimeStamp) {
        // Próba wychwycenia zbyt szybkiego nałożenia ramki:
        auto it = m_sentTimestamps.find(seqId);

        if(it != m_sentTimestamps.end()){
            // Poprzednia ramka z tym numerem nadal czekała na ACK
            it->second.collided = true;
            m_timeoutFrames++;
        }
        m_sentTimestamps[seqId].sentAt = udpTimeStamp;
    }

    void markExecAck(uint16_t seqId) {
        auto now = Clock::now();
        auto it = m_sentTimestamps.find(seqId);
        if (it != m_sentTimestamps.end()) {
            double latencyUs = std::chrono::duration<double, std::micro>(now - it->second.sentAt).count();

            // 0. W przypadku wystąpienia kolizji - brak uwzględnienia w statystykach
            if(it->second.collided){
               return;
            }

            // 1. Zliczanie globalne (Lifetime) - O(1)
            m_lifetimeSum += latencyUs;
            if (latencyUs < m_lifetimeMin) m_lifetimeMin = latencyUs;
            if (latencyUs > m_lifetimeMax) m_lifetimeMax = latencyUs;

            // 2. Wstawianie do bufora kołowego 20k próbek
            if (m_samples.size() < m_maxSamples) {
                m_samples.push_back(latencyUs);
            } else {
                m_samples[m_writeIndex] = latencyUs;
                m_writeIndex = (m_writeIndex + 1) % m_maxSamples;
            }

            m_sentTimestamps.erase(it);
        }
    }

    UsbStatistics calculateStats() const {
        if (m_samples.empty()) return {};

        UsbStatistics stats;

        // Kopiujemy i sortujemy 20k próbek do wyliczenia percentyli
        std::vector<double> sorted = m_samples;
        std::sort(sorted.begin(), sorted.end());

        double sum = std::accumulate(sorted.begin(), sorted.end(), 0.0);
        stats.avgUs = sum / sorted.size();
        stats.minUs = sorted.front();
        stats.maxUs = sorted.back();
        stats.timeoutFrames = m_timeoutFrames;

        // P99 wyliczane precyzyjnie z dużej próbki 20k
        size_t p99Index = static_cast<size_t>((sorted.size() - 1) * 0.99);
        stats.p99Us = sorted[p99Index];

        return stats;
    }

public slots:
    void resetLatencyStats(){
        m_samples.clear();
        double m_lifetimeSum{0.0};
        double m_lifetimeMin{std::numeric_limits<double>::max()};
        double m_lifetimeMax{0.0};
        uint64_t m_timeoutFrames = 0;
    }

private:
    std::unordered_map<uint16_t, PendingFrame> m_sentTimestamps;
    std::vector<double> m_samples;
    size_t m_maxSamples{20000};
    size_t m_writeIndex{0};

    // Statystyki globalne sesji
    double m_lifetimeSum{0.0};
    double m_lifetimeMin{std::numeric_limits<double>::max()};
    double m_lifetimeMax{0.0};
    uint64_t m_timeoutFrames = 0;
};

#endif // LATENCY_TRACKER_H
