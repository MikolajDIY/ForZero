#ifndef CLUSTER_SETTINGS_H
#define CLUSTER_SETTINGS_H

#include <QObject>
#include <QVariantList>

struct ClusterSettings_m{
    Q_GADGET

    Q_PROPERTY(bool SimClusterABS MEMBER SimClusterABS);
    Q_PROPERTY(bool SimClusterTSC MEMBER SimClusterTSC);
    Q_PROPERTY(bool SimClusterEngineTemp MEMBER SimClusterEngineTemp);
    Q_PROPERTY(bool SimClusterAirBag MEMBER SimClusterAirBag);
    Q_PROPERTY(bool SimClusterBreakPads MEMBER SimClusterBreakPads);
    Q_PROPERTY(bool SimClusterScalingRPM MEMBER SimClusterScalingRPM);

public:
    bool SimClusterABS = true;
    bool SimClusterTSC = true;
    bool SimClusterEngineTemp = true;
    bool SimClusterAirBag = true;
    bool SimClusterBreakPads = true;
    bool SimClusterScalingRPM = true;
};
Q_DECLARE_METATYPE(ClusterSettings_m);

#endif // CLUSTER_SETTINGS_H
