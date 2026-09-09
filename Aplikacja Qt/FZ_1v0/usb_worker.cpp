#include <QDebug>

#include "usb_worker.h"
#include "utils.h"

UsbWorker::UsbWorker(QObject *parent) : QObject(parent){
   // m_serialPort = new QSerialPort(this);
}

void UsbWorker::UsbInit(){
    m_serialPort = new QSerialPort(this);
    m_serialPort->setBaudRate(QSerialPort::Baud115200);
}

void UsbWorker::setupPort(const QString &portName){
    if(portName.isEmpty() || !m_serialPort){
        return;
    }

    if(m_serialPort->isOpen()){
        m_serialPort->close();
    }

    m_serialPort->setPortName(portName);
    if(m_serialPort->open(QIODevice::ReadWrite)){
        // 1. Najpierw pełna konfiguracja parametrów
        m_serialPort->setBaudRate(QSerialPort::Baud115200);
        m_serialPort->setDataBits(QSerialPort::Data8);
        m_serialPort->setParity(QSerialPort::NoParity);
        m_serialPort->setStopBits(QSerialPort::OneStop);
        m_serialPort->setFlowControl(QSerialPort::NoFlowControl);

        // 2. NA KONIEC podciągnij linie DTR i RTS
        m_serialPort->setDataTerminalReady(true);
        m_serialPort->setRequestToSend(true);

        disconnect(m_serialPort, &QSerialPort::readyRead, this, &UsbWorker::readIncomigData);
        connect(m_serialPort, &QSerialPort::readyRead, this, &UsbWorker::readIncomigData);

        // Liczenie czasu do wysyłania statystyk USB dla interfejsu w QML
        if(!m_statsTimer){
            m_statsTimer = new QTimer(this);
            connect(m_statsTimer, &QTimer::timeout, this, &UsbWorker::onUSBStatsTimeOut);
            m_statsTimer->start(1000);
        }

        qDebug() << "SUCCESS: Serial Port " << portName << " is Open";
    }
    else{
        qDebug() << "FAIL: While initing Serial Port: " << portName << " Error:" << m_serialPort->errorString();
    }
}

void UsbWorker::processTelemetry(const Cluster_m &data, TimePoint udpTimestamp){
    // Blokada przed nakładaniem się wywołań
    if (m_isProcessing.exchange(true)) {
        return;
    }

    // Zabezpieczenie: ZAWSZE zdejmujemy flagę przy wyjściu z funkcji (RAII pattern)
    struct RAII_Reset {
        std::atomic<bool> &flag;
        ~RAII_Reset() { flag.store(false); }
    } resetter{m_isProcessing};

    if(!m_serialPort || !m_serialPort->isOpen()){
        return;
    }

    static uint8_t frame_number =0;

    uint8_t packet[12];
    packet[0] = 0x02; // START BYTE
    packet[1] = frame_number; // FRAME NUMBER
    packet[2] = 0x00; // COMMAND

    packet[3] = static_cast<uint8_t>(data.speed_m & 0xFF); // SPEED LSB
    packet[4] = static_cast<uint8_t>((data.speed_m >> 8) & 0xFF); // SPEED MSB

    packet[5] = static_cast<uint8_t>(data.rpm_m & 0xFF);        // RPM LSB
    packet[6] = static_cast<uint8_t>((data.rpm_m >> 8) & 0xFF); // RPM MSB

    packet[7] = data.fuel_m; // FUEL
    packet[8] = data.engine_temperature_m; // ENGINE TEMPERATURE
    packet[9] = data.tell_tales; // TELL-TALES

    packet[10] = calculate_crc8(packet, 10); // CRC
    packet[11] = 0x03; // END BYTE

    m_tracker.markSent(frame_number, udpTimestamp);

    frame_number++;
    qint64 bytesWritten = m_serialPort->write(reinterpret_cast<const char*>(packet), sizeof(packet));

    if (bytesWritten == -1) {
        qDebug() << "Błąd zapisu na port COM:" << m_serialPort->errorString();
    }
    else{
        m_packets_sent++;
    }
}

void UsbWorker::readIncomigData(){
    QByteArray response = m_serialPort->readAll();

    const char *data = response.constData();
    int size = response.size();

    for (int i = 0; i < size; ++i) {
        uint8_t byte = static_cast<uint8_t>(data[i]);
        m_tracker.markExecAck(byte);
    }
}

void UsbWorker::onUSBStatsTimeOut(){
    UsbStatistics stats = m_tracker.calculateStats();

    stats.is_connected = (m_serialPort && m_serialPort->isOpen());
    stats.packets_sent = m_packets_sent;

    emit UsbStatsUpdate(stats);
}
