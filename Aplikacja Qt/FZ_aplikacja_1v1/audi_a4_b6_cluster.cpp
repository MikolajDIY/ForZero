#include "audi_a4_b6_cluster.h"

#include <QDebug>
#include "fh6_cluster_transciver.h"

AudiA4B6Cluster::AudiA4B6Cluster(QObject *parent) : QObject(parent) {
    // 1. Tworzymy wątki i workery
    m_usbThread = new QThread(this);
    m_worker = new UsbWorker(); // BEZ UsbInit() tutaj!

    m_udpThread = new QThread(this);
    m_telemetry = new FH6TelemetryReceiver(5300);

    // 2. Najpierw PRZENOSIMY do wątków
    m_worker->moveToThread(m_usbThread);
    m_telemetry->moveToThread(m_udpThread);

    // 3. Dopiero po wystartowaniu wątku inicjalizujemy zasoby sieciowe/sprzętowe!
    connect(m_usbThread, &QThread::started, m_worker, &UsbWorker::UsbInit);
    connect(m_udpThread, &QThread::started, m_telemetry, &FH6TelemetryReceiver::start);

    // 4. Sprzątanie po zakończeniu wątków
    connect(m_usbThread, &QThread::finished, m_worker, &QObject::deleteLater);
    connect(m_udpThread, &QThread::finished, m_telemetry, &QObject::deleteLater);

    // 5. Połączenia danych i sterowania
    connect(this, &AudiA4B6Cluster::requestPortChange, m_worker, &UsbWorker::setupPort);
    connect(this, &AudiA4B6Cluster::sendDataToUsb, m_worker, &UsbWorker::processTelemetry);
    connect(m_worker, &UsbWorker::UsbStatsUpdate, this, &AudiA4B6Cluster::ReciveUsbStats);

    connect(m_telemetry, &FH6TelemetryReceiver::telemetryUpdated, this, &AudiA4B6Cluster::handleTelemetry);

    // 6. Uruchamiamy wątki
    m_usbThread->start();
    m_udpThread->start();
}

AudiA4B6Cluster::~AudiA4B6Cluster() {
    m_usbThread->quit();
    m_usbThread->wait();

    m_udpThread->quit();
    m_udpThread->wait();
}

QStringList AudiA4B6Cluster::availablePorts() const {
    QStringList ports;
    const auto infos = QSerialPortInfo::availablePorts();

    qDebug() << "--- SZUKANIE PORTÓW COM ---";
    for(const QSerialPortInfo &info : infos){
        qDebug() << "Znaleziono port:" << info.portName() << info.description();
        ports.append(info.portName());
    }
    return ports;
}

void AudiA4B6Cluster::setPort(const QString &portName) {
    emit requestPortChange(portName);
}

// Odbiór danych z Telemetri oraz ich parsowanie i przekazanie do USB i UI zgodnie z ustawieniami licznika
void AudiA4B6Cluster::handleTelemetry(const ForzaHorizon6Telemetry &data, const TimePoint udpTimestamp) {
    m_cluster = FH6ClusterTransceiver::AudiA4B6Parser(&data, &m_settings);
    emit clusterDataChanged(); // Aktualizacja UI dla danych z 'm_cluster'
    emit sendDataToUsb(m_cluster, udpTimestamp); // Wysłanie danych dla USB worker
}

void AudiA4B6Cluster::ReciveUsbStats(const UsbStatistics &stats){
    m_usbStats = stats;
    emit usbStatsChanged();
}



