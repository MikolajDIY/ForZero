#ifndef ENGINE_TEMP_SIM_H
#define ENGINE_TEMP_SIM_H

#include <chrono>
#include <algorithm>

#include "fh6_telemetryreciver.h"

class EngineTempSimulator {
private:
    float engineTemp = 20.0f; // Temp. początkowa (np. 20°C w zimnym aucie lub startowo 90°C)
    const float ambientTemp = 20.0f;
    const float targetNormalTemp = 90.0f; // Wskazówka na środku w Audi A4 B6
    std::chrono::steady_clock::time_point lastUpdate;

public:
    EngineTempSimulator(float initialTemp = 90.0f) : engineTemp(initialTemp) {
        lastUpdate = std::chrono::steady_clock::now();
    }

    void Update(const ForzaHorizon6Telemetry& data) {
        auto now = std::chrono::steady_clock::now();
        float dt = std::chrono::duration<float>(now - lastUpdate).count();
        lastUpdate = now;

        // Jeśli wyścig wyłączony lub zgaszony silnik -> płyn chłodzi się do temperatury otoczenia
        if (data.IsRaceOn == 0 || data.CurrentEngineRpm < 200.0f) {
            float coolDownRate = 0.05f; // Powolne stygnięcie zgaszonego silnika
            engineTemp -= (engineTemp - ambientTemp) * coolDownRate * dt;
            return;
        }

        // 1. Współczynnik generowania ciepła (RPM + Gaz + Boost)
        float rpmRatio = data.CurrentEngineRpm / (data.EngineMaxRpm > 0 ? data.EngineMaxRpm : 7000.0f);
        float accelRatio = data.Accel / 255.0f;
        float boostFactor = (data.Boost > 0.0f) ? (1.0f + (data.Boost / 14.7f) * 0.3f) : 1.0f; // Boost dodaje temp.

        // Moc cieplna oddawana do płynu
        float heatGenerated = (0.3f + (rpmRatio * 0.7f * accelRatio * 1.5f)) * boostFactor * 2.5f;

        // 2. Chłodzenie przez pęd powietrza i chłodnicę
        float speedKmh = data.Speed * 3.6f;
        float airFlowCooling = 1.0f + (speedKmh / 100.0f) * 0.8f; // Nadmuch na chłodnicę przy prędkości

        // Termostat: wyższa wydajność chłodzenia powyżej 85°C
        float thermostatFactor = (engineTemp > 85.0f) ? 1.0f : 0.15f;

        float heatDissipated = (engineTemp - ambientTemp) * 0.03f * airFlowCooling * thermostatFactor;

        // 3. Bilans zmian temperatury
        float deltaTemp = (heatGenerated - heatDissipated) * dt;
        engineTemp += deltaTemp;

        // Ograniczenie zakresu wskaźnika (np. od 20°C do 130°C)
        engineTemp = std::clamp(engineTemp, 20.0f, 130.0f);
    }

    uint8_t GetTempForCluster() const {
        return static_cast<uint8_t>(engineTemp);
    }
};

#endif // ENGINE_TEMP_SIM_H
