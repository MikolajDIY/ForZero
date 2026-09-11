#include "fh6_cluster_transciver.h"

// Parsing FH6 telemetry into Audi A4 B6 cluster data
Cluster_m FH6ClusterTransceiver::AudiA4B6Parser(const ForzaHorizon6Telemetry *telemetry, ClusterSettings_m *settings){
    Cluster_m Cluster ={};
    if(telemetry == nullptr){return Cluster;}

    // -------------------------------------
    //  Główna Telemetria
    // -------------------------------------

    // Prędkość
    Cluster.speed_m = telemetry->Speed * 3.6;

    // RPM
    if(settings->SimClusterScalingRPM){
        Cluster.rpm_m = RPM_Scaling(*telemetry);
    }else{
        Cluster.rpm_m = telemetry->CurrentEngineRpm;
    }

    // Paliwo
    Cluster.fuel_m = (uint8_t)(telemetry->Fuel * 100); // Percent value

    // -------------------------------------
    // Opcjonalna Telemetria ( SYMULOWANA )
    // -------------------------------------

    if(settings->SimClusterEngineTemp == true){
        Cluster.engine_temperature_m = EngineTemp_value(*telemetry); // Percent value
    }

    // TELL-TALES TELEMETRY

    // ABS
    if(settings->SimClusterABS == true){
        Cluster.abs_m = ABS_state(*telemetry);
    }
    // TSC
    if(settings->SimClusterTSC == true){
        Cluster.tcr_m = TCR_state(*telemetry);
    }
    // BRAKE_PADS
    if(settings->SimClusterBreakPads == true){
        // PLACE HOLDER for BREAKE PADS USE SIMULATION
    }
    //AIR_BAG
    if(settings->SimClusterAirBag == true){
        Cluster.airbag_m = AIR_BAG_state(*telemetry);
    }

    Cluster.brakes_m = HEAND_BRAKE_state(*telemetry);


    // Set Tell-Tales
    SetTellTales(Cluster, *settings);

    return Cluster;
}

void FH6ClusterTransceiver::SetTellTales(Cluster_m &Cluster, ClusterSettings_m &settings){

    // Pobieramy aktualny czas systemowy w milisekundach
    auto now = std::chrono::steady_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();

    // (ms / 100) zmienia wartość co 100ms (np. 0, 1, 2, 3...)
    // % 2 daje naprzemiennie true / false (0, 1, 0, 1...)
    bool toggleState = (ms / 100) % 2 == 0;

    // ABS
    if (Cluster.abs_m) {
        if (toggleState) {
            Cluster.tell_tales |= (1 << 0);  // ON
        } else {
            Cluster.tell_tales &= ~(1 << 0); // OFF
        }
    } else {
        Cluster.tell_tales &= ~(1 << 0);
    }

    // TCR
    if(Cluster.tcr_m){
        if(toggleState){
            Cluster.tell_tales |= (1<<1); // ON
        }else{
            Cluster.tell_tales &= ~(1<<1); // OFF
        }
    } else{
        Cluster.tell_tales &= ~(1<<1);
    }

    // HEAND BRAKE
    if(Cluster.brakes_m){
        Cluster.tell_tales |= (1<<2); // ON
    }else{
        Cluster.tell_tales &= ~(1<<2); // OFF
    }

    // AIR BAG
    if((Cluster.airbag_m || isAirBagTripped) && settings.SimClusterAirBag){
        Cluster.tell_tales |= (1<<7); // ON - Zatrzask
        isAirBagTripped = true;
    }else{
        Cluster.tell_tales &= ~(1<<7); // OFF
        isAirBagTripped = false; // Zerowanie zatrzasku
    }
}

bool FH6ClusterTransceiver::ABS_state( const ForzaHorizon6Telemetry& data){

        // 1. Sprawdzamy, czy gracz w ogóle używa hamulca zasadniczego (np. powyżej ~15% wciśnięcia)
        if (data.Brake < 40) {
            return false;
        }

        // 2. Zabezpieczenie prędkości: ABS nie interweniuje przy niskich prędkościach.
        // data.Speed jest w m/s. 5.0f to około 18 km/h. Poniżej tej prędkości koła mogą
        // się zablokować bez włączania ABS-u.
        if (data.Speed < 5.0f) {
            return false;
        }

        // 3. Sprawdzamy uślizg wzdłużny opon.
        // Wartość powyżej ~0.85 oznacza, że koło traci przyczepność przy hamowaniu,
        // a system ABS będzie próbował je "odratować".
        const float slipThreshold = 0.85f;

        bool frontSlip = (std::abs(data.TireSlipRatioFrontLeft) > slipThreshold) ||
                         (std::abs(data.TireSlipRatioFrontRight) > slipThreshold);

        bool rearSlip = false;

        // 4. Ignorujemy tylne koła, jeśli gracz używa hamulca ręcznego!
        // Ręczny wymusza zablokowanie kół, co nie jest błędem hamulców i ABS na to nie reaguje.
        if (data.HandBrake < 10) {
            rearSlip = (std::abs(data.TireSlipRatioRearLeft) > slipThreshold) ||
                       (std::abs(data.TireSlipRatioRearRight) > slipThreshold);
        }

        // Jeśli podczas szybkiej jazdy i wciśniętego hamulca koła wpadają w poślizg -> ABS działa
        return frontSlip || rearSlip;

}

bool FH6ClusterTransceiver::TCR_state(const ForzaHorizon6Telemetry& data) {
    // 1. TCS działa tylko, gdy gracz dodaje gazu (np. > 15%)
    if (data.Accel < 40) {
        return false;
    }

    // 2. Próg utraty trakcji w Forzy (0.0 = 100% grip, 1.0 = limit przyczepności)
    // Jeśli autko buksuje, CombinedSlip przekracza 0.9 - 1.0
    const float tcsThreshold = 0.75f;

    bool frontSpin = (std::abs(data.TireSlipRatioFrontLeft) > tcsThreshold) ||
                     (std::abs(data.TireSlipRatioFrontRight) > tcsThreshold);

    bool rearSpin  = (std::abs(data.TireSlipRatioRearLeft) > tcsThreshold) ||
                    (std::abs(data.TireSlipRatioRearRight) > tcsThreshold);

    // 3. Sprawdzamy osie w zależności od napędu
    switch (data.DrivetrainType) {
    case 0:  // FWD
        return frontSpin;
    case 1:  // RWD
        return rearSpin;
    case 2:  // AWD
    default:
        return frontSpin || rearSpin;
    }
}

bool FH6ClusterTransceiver::HEAND_BRAKE_state(const ForzaHorizon6Telemetry& data){
    if(data.HandBrake > 0){
        return true;
    }
    else{
        return false;
    }
}

bool FH6ClusterTransceiver::AIR_BAG_state(const ForzaHorizon6Telemetry& data) {
    // Przeliczamy przyspieszenie z m/s^2 na jednostki g
    float gForceZ = std::abs(data.AccelerationZ) / 9.81f;
    float gForceX = std::abs(data.AccelerationX) / 9.81f;

    // Wypadek czołowy: powyżej 12g LUB mocny strzał z boku: powyżej 10g
    const float frontCrashThresholdG = 12.0f;
    const float sideCrashThresholdG  = 10.0f;

    if (gForceZ > frontCrashThresholdG || gForceX > sideCrashThresholdG) {
        return true; // Strzał poduszki!
    }

    return false;
}

uint8_t FH6ClusterTransceiver::EngineTemp_value(const ForzaHorizon6Telemetry& data){
    static float engineTemp = 90.0f;
    static auto lastUpdate = std::chrono::steady_clock::now();

    auto now = std::chrono::steady_clock::now();
    float dt = std::chrono::duration<float>(now - lastUpdate).count();
    lastUpdate = now;

    if (dt > 1.0f) dt = 0.016f;

    // === 1. FIZYKA SYMULACJI ===
    if (data.IsRaceOn == 0 || data.CurrentEngineRpm < 200.0f) {
        // Stygnięcie w menu / po wyłączeniu gry
        engineTemp -= (engineTemp - 20.0f) * 0.05f * dt;
    }
    else {
        // Generowanie ciepła
        float rpmRatio = data.CurrentEngineRpm / (data.EngineMaxRpm > 0 ? data.EngineMaxRpm : 7000.0f);
        float accelRatio = data.Accel / 255.0f;
        float boostPSI = (data.Boost > 0.0f) ? data.Boost : 0.0f;

        float heatGenerated = 0.5f + (rpmRatio * 2.2f) + (accelRatio * 2.0f) + (boostPSI * 0.05f);

        // Chłodzenie i dwustanowy termostat (ON/OFF sztywno przy 90°C)
        float speedKmh = data.Speed * 3.6f;
        float airFlowCooling = 1.0f + (speedKmh / 50.0f);

        // Powyżej 90°C otwórz (1.0), poniżej 90°C zamknij (0.05)
        float thermostat = (engineTemp > 90.0f) ? 1.0f : 0.05f;

        float heatDissipated = (engineTemp - 20.0f) * 0.03f * airFlowCooling * thermostat;

        engineTemp += (heatGenerated - heatDissipated) * dt;
    }

    // Ograniczenie fizycznej temperatury do zakresu 20°C - 130°C
    engineTemp = std::clamp(engineTemp, 20.0f, 130.0f);

    // === 2. BEZPOŚREDNIE PRZELICZENIE NA PROCENTY ===
    // 50°C -> 0% | 90°C -> 50% | 130°C -> 100%
    float percent = ((engineTemp - 50.0f) / 80.0f) * 100.0f;
    percent = std::clamp(percent, 0.0f, 100.0f);

    return static_cast<uint8_t>(percent);
}

uint16_t FH6ClusterTransceiver::RPM_Scaling(const ForzaHorizon6Telemetry& data){
    // Zabezpieczenie przed dzieleniem przez zero oraz błędnymi danymi
    if (data.EngineMaxRpm == 0) {
        return 0;
    }

    // Proporcjonalne mapowanie zakresu [0, EngineMaxRpm] -> [0, maxRPM]
    float ratio = static_cast<float>(maxRPM) / data.EngineMaxRpm;
    uint32_t scaled_rpm = static_cast<uint32_t>(data.CurrentEngineRpm * ratio);

    // Ograniczenie wartości
    if (scaled_rpm > maxRPM) {
        scaled_rpm = maxRPM;
    }

    return static_cast<uint16_t>(scaled_rpm);
}
