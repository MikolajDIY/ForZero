#include <SPI.h>
#include <mcp_can.h>

const int SPI_CS_PIN = 17;
MCP_CAN CAN(SPI_CS_PIN);

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  SPI.begin();
  
  delay(3000);
  Serial.println("--- AUDI RB4 FULL TEST ---");

  // Inicjalizacja CAN - 500kbps, kwarc 8MHz
  if (CAN.begin(MCP_NORMAL, CAN_500KBPS, MCP_8MHZ) == CAN_OK) {
    Serial.println("CAN OK");
  } else {
    Serial.println("Błąd SPI/CAN!");
    while (1);
  }
  CAN.setMode(MCP_NORMAL);
    
}

void loop() {
  static int cout = 0;
  // -------------------------------------------------------------------------
  // 1. RAMKA 0x280 - SILNIK (Motor 1)
  // Steruje: Obrotomierzem, kontrolką EPC, Check Engine, Immobilizer
  // Obroty: Bajty 2 i 3 (Wartość = RPM * 4). Poniżej: 0x40 0x1F = 8000 = 2000 RPM
  // Bajt 5: Flagi (0x02 = Check Engine OFF, 0x01 = EPC OFF)
  // -------------------------------------------------------------------------
  byte msg_rpm[8] = {0x49, 0x0E, 0x40, 0x1F, 0x00, 0x03, 0x00, 0x00};


  // -------------------------------------------------------------------------
  // 2. RAMKA 0x288 - SILNIK (Motor 2)
  // Steruje: Kontrolką Tempomatu (Cruise Control)
  // Bajt 5 (Bit 0) = Tempomat (1 = Wł, 0 = Wył)
  // -------------------------------------------------------------------------
  byte msg_motor2[8] = {0x00, 0xB8, 0x4B, 0x01, 0x08, 0x01, 0x00, 0x00};


  // -------------------------------------------------------------------------
  // 3. RAMKA 0x1A0 - HAMULCE/ABS (Brakes 1)
  // Steruje: Prędkościomierzem (Prędkość pojazdu), Kontrolką ABS, Kontrolką ESP
  // Prędkość: Bajty 1 i 2 (Wartość = km/h * 100). 
  // Poniżej: 0x27 0x10 = 10000 = 100 km/h
  // Flagi ABS/ESP są na bajtach 0 i 3.
  // -------------------------------------------------------------------------
  byte msg_speed[8] = {0x18, 0x10, 0x27, 0x00, 0x00, 0x00, 0x00, 0x00};


  // -------------------------------------------------------------------------
  // 4. RAMKA 0x5A0 - HAMULCE/ABS (Brakes 2)
  // Steruje: Zgaszeniem kontrolki ręcznego/awarii hamulców (często pika na żółto)
  // Symuluje poprawny stan systemu (brak błędu EBD/ABS)
  // -------------------------------------------------------------------------
  byte msg_brakes2[8] = {0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};


  // -------------------------------------------------------------------------
  // 5. RAMKA 0x5A4 - GATEWAY/IMMOBILIZER
  // W VAG czasami wymagana, aby licznik nie "krzyczał", że brak komunikacji
  // z bramką sieciową (CAN Gateway).
  // -------------------------------------------------------------------------
  byte msg_gateway[8] = {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

  // Ramka 0x050 - Airbag (Wysyłana co 100ms)
  // W VAG same zera oznaczają brak usterek w systemie
  byte msg_airbag[8] = {0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};


  
  // Wysyłanie pakietu danych (co 50ms = 20Hz)
  CAN.sendMsgBuf(0x280, 0, 8, msg_rpm);
  CAN.sendMsgBuf(0x288, 0, 8, msg_motor2);
  CAN.sendMsgBuf(0x1A0, 0, 8, msg_speed);
  CAN.sendMsgBuf(0x5A0, 0, 8, msg_brakes2);
  CAN.sendMsgBuf(0x5A4, 0, 8, msg_gateway);
  if(cout == 2){
    CAN.sendMsgBuf(0x050, 0, 8, msg_airbag);
    cout =0;
  }

  // Mruganie diodą jako potwierdzenie działania pętli
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));

  // Opóźnienie 50ms - standard VAG dla płynnego ruchu wskazówek
  delay(50); 
  cout ++;
}