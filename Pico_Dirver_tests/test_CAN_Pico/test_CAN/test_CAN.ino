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
  static int cout_1A0 = 0;
  static int cout_2A0 = 0;
  static int cout_280 = 0;
  static int cout_288 = 0;
  // -------------------------------------------------------------------------
  // 1. RAMKA 0x280 - SILNIK (Motor 1)
  // Steruje: Obrotomierzem, kontrolką EPC, Check Engine, Immobilizer
  // Obroty: Bajty 2 i 3 (Wartość = RPM * 4). Poniżej: 0x40 0x1F = 8000 = 2000 RPM
  byte msg_rpm[8] = {0x00, 0x00, 0x34, 0x0B, 0x00, 0x12, 0x00, 0x00};
  // 2. RAMKA 0x288 - SILNIK (Motor 2)
   byte msg_motor2[8] = {0xC0, 0x10, 0x8A, 0x00, 0x02, 0x00, 0x01, 0x00};
  // 3. RAMKA 0x1A0 
  byte msg_speed[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  // 4. RAMKA 0x2A0
  byte msg_breaks[8] = {0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00};
  

  if(cout_280 == 2){
    CAN.sendMsgBuf(0x280, 0, 8, msg_rpm);
    cout_280 = 0;
  }
  if(cout_288 == 5){
    CAN.sendMsgBuf(0x288, 0, 8, msg_motor2);
    cout_288 =0;
  }
  if(cout_1A0 == 2){
    CAN.sendMsgBuf(0x1A0, 0, 8, msg_speed);
    cout_1A0 = 0;
  }
  if(cout_2A0 == 5){
    CAN.sendMsgBuf(0x5A0, 0, 8, msg_breaks);
    cout_2A0 =0;
  }
  if(cout_2A0 == 5){
    CAN.sendMsgBuf(0x0A0, 0, 8, msg_breaks);
    cout_2A0 =0;
  }

  // Mruganie diodą jako potwierdzenie działania pętli
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));

  // Opóźnienie 50ms - standard VAG dla płynnego ruchu wskazówek
  delay(10); 
  cout_280 ++;
  cout_288 ++;
  cout_1A0 ++;
  cout_2A0 ++;
}