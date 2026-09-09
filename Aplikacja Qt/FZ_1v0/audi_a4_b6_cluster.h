#ifndef AUDI_A4_B6_CLUSTER_H
#define AUDI_A4_B6_CLUSTER_H

#include <QObject>
#include <QtQml/qqmlregistration.h>
#include <QThread>

#include "usb_worker.h"
#include "fh6_telemetryreciver.h"
#include "cluster.h"
#include "cluster_settings.h"
#include "utils.h"


// Class sending data for cluster by USB
class AudiA4B6Cluster : public QObject {
    Q_OBJECT

    Q_PROPERTY(ClusterSettings_m settings READ settings WRITE setSettings);

    Q_PROPERTY(UsbStatistics usbStats READ usbStats NOTIFY usbStatsChanged);

    Q_PROPERTY(Cluster_m cluster READ cluster NOTIFY clusterDataChanged);

public:
    explicit AudiA4B6Cluster(QObject *parent = nullptr);

    ~AudiA4B6Cluster();

    // Te metody QML wywołuje bezpośrednio w wątku głównym:
    Q_INVOKABLE QStringList availablePorts() const;
    Q_INVOKABLE void setPort(const QString &portName);

    ClusterSettings_m settings() const {return m_settings;}
    Cluster_m cluster() const {return m_cluster;}
    UsbStatistics usbStats() const {return m_usbStats;}

    void setSettings(ClusterSettings_m &s){m_settings = s;}

public slots:
    void handleTelemetry(const ForzaHorizon6Telemetry &data, TimePoint udpTimestamp); // Parsowanie danych dla UI oraz USB i ich przekazanie
    void ReciveUsbStats(const UsbStatistics &stats);

signals:
    void requestPortChange(const QString &portName);
    void sendDataToUsb(const Cluster_m &data, TimePoint udpTimestamp);

    void usbStatsChanged();
    void clusterDataChanged();

private:
    QThread *m_usbThread;
    UsbWorker *m_worker;

    QThread *m_udpThread;
    FH6TelemetryReceiver *m_telemetry;

    Cluster_m m_cluster;
    UsbStatistics m_usbStats;
    ClusterSettings_m m_settings;
};
#endif // AUDI_A4_B6_CLUSTER_H
