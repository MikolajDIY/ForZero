#include "fh6_telemetryreciver.h"

#include <QHostAddress>
#include <QDebug>
#include <QMutexLocker>

FH6TelemetryReceiver::FH6TelemetryReceiver(quint16 port, QObject *parent)
    : QObject(parent), m_port(port) {}

void FH6TelemetryReceiver::start() {
    if (!udpSocket) {
        udpSocket = new QUdpSocket(this);
        guiUpdateTimer = new QTimer(this);

        connect(udpSocket, &QUdpSocket::readyRead, this, &FH6TelemetryReceiver::processPendingDatagrams);
        connect(guiUpdateTimer, &QTimer::timeout, this, &FH6TelemetryReceiver::sendBufferedTelemetry);

        udpSocket->bind(QHostAddress::AnyIPv4, m_port, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);
        // guiUpdateTimer->start(10); // 100 Hz
    }
}

void FH6TelemetryReceiver::processPendingDatagrams() {
    while (udpSocket->hasPendingDatagrams()) {
        // Pobranie czasu otrzymania pakietu UDP
        auto udpTimestamp = std::chrono::high_resolution_clock::now();

        QByteArray datagram;
        datagram.resize(udpSocket->pendingDatagramSize());
        udpSocket->readDatagram(datagram.data(), datagram.size());

        if (datagram.size() >= static_cast<int>(sizeof(ForzaHorizon6Telemetry))) {
            const auto *telemetry = reinterpret_cast<const ForzaHorizon6Telemetry*>(datagram.constData());
            if (telemetry->IsRaceOn == 1 || telemetry->IsRaceOn == 0) {
                // Bezpośredni prosty zapis
                m_latestClusterTelemetry = *telemetry;
                m_latestUdpTimestamp = udpTimestamp;

                emit telemetryUpdated(m_latestClusterTelemetry, m_latestUdpTimestamp);
                m_hasNewData.store(true, std::memory_order_release);
            }
        }
    }
}

void FH6TelemetryReceiver::sendBufferedTelemetry() {
    // Sprawdzamy atomowo bez zajmowania Mutexa
    if (m_hasNewData.exchange(false, std::memory_order_acq_rel)) {
        emit telemetryUpdated(m_latestClusterTelemetry, m_latestUdpTimestamp);
    }
}
