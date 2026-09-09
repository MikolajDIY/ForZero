#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QThread>

#include "audi_a4_b6_cluster.h"
#include "cluster_settings.h"
#include "utils.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    // Rejestracja typu dla sygnału
    qRegisterMetaType<Cluster_m>("Cluster_m");
    qRegisterMetaType<UsbStatistics>("UsbStatistics");
    qRegisterMetaType<ClusterSettings_m>("ClusterSettings_m");

    AudiA4B6Cluster *audiCluster = new AudiA4B6Cluster();

    QQmlApplicationEngine engine;

    // Udostępniamy odbiornik do QML przed załadowaniem interfejsu
    engine.rootContext()->setContextProperty("audiCluster", audiCluster);

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);

    engine.loadFromModule("Fz_udp", "Main");



    return app.exec();
}
