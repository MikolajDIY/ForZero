#ifndef UTILS_H
#define UTILS_H

#include <cstdint>
#include <QObject>
#include <QVariantList>

inline uint8_t calculate_crc8(const uint8_t *data, uint8_t len) {
    uint8_t crc = 0x00;
    for (uint8_t i = 0; i < len; i++) {
        crc ^= data[i];
        for (uint8_t j = 0; j < 8; j++) {
            if (crc & 0x80) {
                crc = (crc << 1) ^ 0x07;
            } else {
                crc <<= 1;
            }
        }
    }
    return crc;
}

struct UsbStatistics {
    Q_GADGET

    // Udostępnienie pól dla QML
    Q_PROPERTY(bool is_connected MEMBER is_connected)
    Q_PROPERTY(uint64_t packets_sent MEMBER packets_sent)
    Q_PROPERTY(double minUs MEMBER minUs)
    Q_PROPERTY(double maxUs MEMBER maxUs)
    Q_PROPERTY(double avgUs MEMBER avgUs)
    Q_PROPERTY(double p99Us MEMBER p99Us)

public:
    bool is_connected = false;
    uint64_t packets_sent = 0;
    double minUs = 0;
    double maxUs = 0;
    double avgUs = 0;
    double p99Us = 0;

    // Q_INVOKABLE pozwala wywołać tę metodę bezpośrednio z QML
    Q_INVOKABLE QVariantList toList() const {
        return QVariantList{
            QVariantMap{{"name", "MIN"}, {"value", minUs}},
            QVariantMap{{"name", "MAX"}, {"value", maxUs}},
            QVariantMap{{"name", "AVG"},   {"value", avgUs}},
            QVariantMap{{"name", "P99"},   {"value", p99Us}}
        };
    }
};

Q_DECLARE_METATYPE(UsbStatistics)

#endif // UTILS_H
