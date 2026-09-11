#ifndef USB_WORKER_H
#define USB_WORKER_H
#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QTimer>

#include <cluster.h>
#include <utils.h>
#include <latency_tracker.h>

using TimePoint = std::chrono::high_resolution_clock::time_point;

class UsbWorker : public QObject {
    Q_OBJECT
public:
    explicit UsbWorker(QObject *parent = nullptr);
    void UsbInit();

signals:
    void UsbStatsUpdate(const UsbStatistics &stats);

public slots:
    void setupPort(const QString &portName);
    void processTelemetry(const Cluster_m &data, TimePoint udpTimestamp);

private slots:
    void readIncomigData();
    void onUSBStatsTimeOut();

private:
    // Komunikacja USB
    QSerialPort *m_serialPort = nullptr;
    std::atomic<bool> m_isProcessing{false};

    // Statystyki USB
    uint64_t m_packets_sent = 0;
    QTimer *m_statsTimer = nullptr;
    LatencyTracker m_tracker;
};

#endif // USB_WORKER_H
