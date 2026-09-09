/*
 * cluster.c
 *
 *  Created on: 13 sie 2026
 *      Author: rataj
 */

#include "cluster.h"
#include "tell_tales.h"
#include "fuel_and_temp.h"
#include "CAN_motor.h"
#include "speed.h"
#include "tell_tales.h"
#include "stdbool.h"

#include "usbd_cdc_if.h"

volatile Cluster g_cluster = { .indicators = SENSOR_BIT};
// Deleting error during cluster start (no data pack and no rpm)

// CLUSTER TEST
static uint8_t test_active = 0;
static uint32_t test_start_time = 0;
static uint32_t last_send_time = 0;

// Funkcja CRC-8 (XOR / Polynomial 0x07) przeliczana dla pierwszych 10 bajtów
static uint8_t calculate_crc8(const uint8_t *data, uint8_t len) {
    uint8_t crc = 0x00;
    for (uint8_t i = 0; i < len; i++) {
        crc ^= data[i];
        for (uint8_t j = 0; j < 8; j++) {
            if (crc & 0x80) {
                crc = (crc << 1) ^ 0x07;
            } else {
                crc <<= 1;
            }
        }
    }
    return crc;
}

bool parse_packet(const uint8_t *raw_buf, volatile Cluster *cluster) {
    const RawPacket_t *pkt = (const RawPacket_t *)raw_buf;

    // Statyczne zmienne do śledzenia ciągłości ramek
	static uint8_t expected_pkt_num = 0;
	static bool is_first_packet = true;


    // 1. Walidacja bajtu startu i stopu

    if (pkt->start_byte != PACKET_START_BYTE || pkt->stop_byte != PACKET_STOP_BYTE) {

        return false;
    }

    // 2. Walidacja CRC (liczona od bajtu 0 do 9, czyli pierwsze 10 bajtów)
    uint8_t computed_crc = calculate_crc8(raw_buf, 10);
    if (computed_crc != pkt->crc) {

        return false; // Błąd spójności danych
    }

    // 3. Sprawdzanie ciągłości numerycznej ramek
	if (is_first_packet) {
		// Pierwsza ramka po starcie/resecie - przyjmujemy jej numer jako bazę
		expected_pkt_num = (uint8_t)(pkt->pkt_num + 1);
		is_first_packet = false;
	}
	else if (pkt->pkt_num != expected_pkt_num) {

		// Resynchronizacja: ustawiamy kolejny oczekiwany numer na bieżący + 1
		expected_pkt_num = (uint8_t)(pkt->pkt_num + 1);

		// Zależnie od wymagań:
		// return false; // jeśli chcesz odrzucić ramkę przysłaną poza kolejnością
	}
	else {
		// Numer ramki poprawny – wyliczamy oczekiwany numer dla KOLEJNEJ paczki
		expected_pkt_num = (uint8_t)(pkt->pkt_num + 1);
	}

    // 4. Przepisanie i rekonstrukcja wartości
    cluster->last_pkt_num = pkt->pkt_num;
    cluster->command      = pkt->command;

    // Uruchomienie testu
    if(cluster->command == 0x01){start_cluster_test();}

    cluster->speed        = (uint16_t)((pkt->speed_msb << 8 )| pkt->speed_lsb);
    // FUSE
    if(cluster->speed >= 260){cluster->speed = 260;}

    cluster->rpm 		  = (uint16_t)((pkt->rpm_msb << 8) | pkt->rpm_lsb);
    // FUSE
    if(cluster->rpm >= 8000){cluster->rpm = 8000;}

    cluster->fuel_level   = pkt->fuel_level;
    cluster->engine_temp  = pkt->engine_temp;
    cluster->indicators   = pkt->indicators;

    // Oil level sensor needs diferent stages in case of rpm
	if(cluster->rpm == 0){
		cluster->indicators ^= SENSOR_BIT;
	}

    return true;
}

// --------- CLUSTER TEST ----------

void start_cluster_test(void) {
    test_active = 1;
    test_start_time = HAL_GetTick();
    last_send_time = 0;
}

uint8_t is_cluster_test_active(){
	return test_active;
}

void process_cluster_test(void) {
    if (!test_active) return;

    uint32_t now = HAL_GetTick();

    if (now - test_start_time <= 2000) {
        if (now - last_send_time >= 50) {
            last_send_time = now;
            Send_RPM(8000);
            Send_Speed(260);
            Send_TellTales(&(uint8_t){0b11110111});
        }
    } else {
        Send_RPM(0);
        Send_Speed(0);
        Send_Fuel_lvl(100);
        Send_Engine_Temperature(0);
        TellTales_Init();
        test_active = 0;
    }
}

