/*
 * cluster.h
 *
 *  Created on: 13 sie 2026
 *      Author: rataj
 */

#ifndef INC_CLUSTER_H_
#define INC_CLUSTER_H_

#include <stdbool.h>
#include "main.h"

#define PACKET_START_BYTE 0x02  // Twój bajt startu (np. STX = 0x02 lub 0xAA)
#define PACKET_STOP_BYTE  0x03  // Twój bajt stopu (np. ETX = 0x03 lub 0x55)
#define PACKET_LEN        13

// Struktura reprezentująca odebraną surową ramkę z USB
typedef struct __attribute__((packed)) {
    uint8_t start_byte;   // 1. Bajt startu
    uint8_t pkt_num_lsb;  // 2. Numer paczki LSB
    uint8_t pkt_num_msb;  // 3. Numer paczki MSB
    uint8_t command;      // 4. Polecenie z PC
    uint8_t speed_lsb;    // 5. Prędkość LSB
    uint8_t speed_msb;    // 6. Prędkość MSB
    uint8_t rpm_lsb;      // 7. RPM LSB
    uint8_t rpm_msb;      // 8. RPM MSB
    uint8_t fuel_level;   // 9. Poziom paliwa
    uint8_t engine_temp;  // 10. Temperatura silnika
    uint8_t indicators;   // 11. Kontrolki (bitfield/flagi)
    uint8_t crc;          // 12. CRC
    uint8_t stop_byte;    // 13. Bajt stopu
} RawPacket_t;


// Docelowa struktura danych zegarów
typedef struct {
    uint16_t  last_pkt_num;
    uint8_t  command;
    uint16_t speed;       // Prędkość całkowita w km/h
    uint16_t rpm;         // Obroty silnika w RPM
    uint8_t  fuel_level;  // Poziom paliwa (%)
    uint8_t  engine_temp; // Temp. silnika (°C)
    uint8_t  indicators;  // Maska bitowa kontrolek
} Cluster;

extern volatile Cluster g_cluster;
// PARSOWANIE DANYCH
bool parse_packet(const uint8_t *raw_buf, volatile Cluster *cluster);

// TEST LICZNIKA
void start_cluster_test(void);
void process_cluster_test(void);
uint8_t is_cluster_test_active();

#endif /* INC_CLUSTER_H_ */
