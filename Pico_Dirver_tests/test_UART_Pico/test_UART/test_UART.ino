// Tablica na 19 bajtów (bez znacznika SOF)
uint8_t rx_buffer[19];
unsigned long last_rx_time = 0;

// Zmienne do maszyny stanów (nieblokujący odczyt UART)
int rx_index = 0;
bool receiving = false;

// Zmienna do przechowywania odczytanej prędkości
int aktualna_predkosc = 0;

void setup() {
  // Dla Pi Pico 'Serial' to komunikacja przez wbudowane USB.
  // Jeśli używasz sprzętowych pinów UART (np. GP0, GP1), zamień 'Serial' na 'Serial1'.
  Serial.begin(115200);
  
  // Konfiguracja wbudowanej diody.
  // Uwaga: Jeśli używasz Pi Pico W, upewnij się, że masz wybraną odpowiednią 
  // płytkę w IDE, ponieważ dioda jest tam podpięta pod układ bezprzewodowy CYW43439.
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  // 1. Zbieramy dane bez blokowania głównej pętli (odczytujemy wszystko co jest w buforze sprzętowym)
  while (Serial.available() > 0) {
    uint8_t incomingByte = Serial.read();
    
    if (!receiving) {
      // Szukamy znacznika początku (255)
      if (incomingByte == 255) {
        receiving = true;
        rx_index = 0; // Zerujemy indeks dla nowej ramki
      }
    } else {
      // Zapisujemy kolejne bajty do bufora
      rx_buffer[rx_index++] = incomingByte;
      
      // Sprawdzamy, czy wczytało się dokładnie 19 bajtów
      if (rx_index == 19) {
        receiving = false; // Koniec ramki, resetujemy stan
        
        // --- KROK A: Weryfikacja sumy kontrolnej ---
        uint8_t suma_obliczona = 0;
        
        // Sumujemy bajty z danymi (indeksy 0-17 w naszym rx_buffer)
        for (int i = 0; i < 18; i++) {
          suma_obliczona += rx_buffer[i];
        }
        suma_obliczona = suma_obliczona % 250;
        
        // Ostatni bajt w buforze (indeks 18) to suma nadesłana z Windowsa
        if (suma_obliczona == rx_buffer[18]) {
          
          // --- KROK B: Rozpakowanie poprawnych danych ---
          int setki = rx_buffer[0];
          int reszta = rx_buffer[1];
          
          // Odtworzenie oryginalnej prędkości
          aktualna_predkosc = (setki * 100) + reszta;
          
          // Aktualizacja znacznika czasu i mrugnięcie diodą
          last_rx_time = millis();
          digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
          
          // --- KROK C: Potwierdzenie odbioru do Windowsa (ACK) ---
          Serial.write((uint8_t)0x00);
        }
      }
    }
  }
  if (millis() - last_rx_time > 100) {
    digitalWrite(LED_BUILTIN, LOW); // Wyłącz diodę
    
    // aktualna_predkosc = 0; // Opcjonalnie: wyzeruj wskaźnik dla bezpieczeństwa
  }
  
  // --- TUTAJ AKTUALIZUJESZ LICZNIK ---
  // Dzięki wyeliminowaniu blokującego readBytes(), Twój kod sterujący 
  // sprzętem może działać tutaj z maksymalną wydajnością rdzenia RP2040.
}