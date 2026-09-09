#ifndef FH6_CLUSTER_TRANSCIVER_H
#define FH6_CLUSTER_TRANSCIVER_H

#include <QObject>
#include <cmath>
#include <audi_a4_b6_cluster.h>
#include <fh6_telemetryreciver.h>

#include "cluster.h"

class FH6ClusterTransceiver{

public:
    static Cluster_m AudiA4B6Parser(const ForzaHorizon6Telemetry *telemetry, ClusterSettings_m *settings);

private:
    static bool ABS_state(const ForzaHorizon6Telemetry& data);
    static bool TCR_state(const ForzaHorizon6Telemetry& data);
    static bool HEAND_BRAKE_state(const ForzaHorizon6Telemetry& data);
    static bool AIR_BAG_state(const ForzaHorizon6Telemetry& data);

    static void SetTellTales(Cluster_m& Cluster, ClusterSettings_m& settings);

    static uint8_t EngineTemp_value(const ForzaHorizon6Telemetry& data);
    static uint16_t RPM_Scaling(const ForzaHorizon6Telemetry& data);

    // Zatrzaski
    inline static bool isAirBagTripped = false;

    // maksymalne i minimalne obroty wykorzystanego licznika
    inline static uint16_t maxRPM = 8000;
    inline static uint16_t minRPM = 0;

};

#endif // FH6_CLUSTER_TRANSCIVER_H
