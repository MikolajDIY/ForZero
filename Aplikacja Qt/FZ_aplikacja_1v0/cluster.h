#ifndef CLUSTER_H
#define CLUSTER_H

#include <cstdint>
#include <QObject>

struct Cluster_m {
    Q_GADGET

    Q_PROPERTY(uint16_t speed_m MEMBER speed_m)
    Q_PROPERTY(uint16_t rpm_m MEMBER rpm_m)
    Q_PROPERTY(uint8_t fuel_m MEMBER fuel_m)
    Q_PROPERTY(uint8_t engine_temperature_m MEMBER engine_temperature_m)

    Q_PROPERTY(bool abs_m MEMBER abs_m)
    Q_PROPERTY(bool tcr_m MEMBER tcr_m)
    Q_PROPERTY(bool coolant_lvl_m MEMBER coolant_lvl_m)
    Q_PROPERTY(bool airbag_m MEMBER airbag_m)
    Q_PROPERTY(bool sensor_m MEMBER sensor_m)
    Q_PROPERTY(bool brake_pads_m MEMBER brake_pads_m)
    Q_PROPERTY(bool wiper_fluid_m MEMBER wiper_fluid_m)
    Q_PROPERTY(bool brakes_m MEMBER brakes_m)

public:
    uint16_t speed_m = 0;
    uint16_t rpm_m = 0;
    uint8_t fuel_m = 0; // percent
    uint8_t engine_temperature_m = 0; // percent

    // Errors Tell-Tales
    bool abs_m = false;
    bool tcr_m = false;
    bool coolant_lvl_m = false;
    bool airbag_m = false;
    bool sensor_m = false;
    bool brake_pads_m = false;
    bool wiper_fluid_m = false;
    bool brakes_m = false;

    uint8_t tell_tales = 0; // All Tell-Talles states for USB_WORKER
};

Q_DECLARE_METATYPE(Cluster_m)

#endif // CLUSTER_H
