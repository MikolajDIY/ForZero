#include <iostream>
#include <windows.h>
#include <chrono>
#include <thread>
#include <cstdint>

int main() {
    // 1. Ustawienia portu
    std::string portName = "\\\\.\\COM3";

    HANDLE hSerial = CreateFileA(
        portName.c_str(),
        GENERIC_READ | GENERIC_WRITE,
        0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL
    );

    if (hSerial == INVALID_HANDLE_VALUE) {
        std::cerr << "Blad: Nie mozna otworzyc portu " << portName << "\n";
        return 1;
    }

    // 2. Konfiguracja UART (115200 Baud)
    DCB dcbSerialParams = {0};
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    if (!GetCommState(hSerial, &dcbSerialParams)) {
        std::cerr << "Blad pobierania stanu portu.\n";
        CloseHandle(hSerial);
        return 1;
    }

    dcbSerialParams.BaudRate = CBR_115200; // Podniesiona prêdkoœæ
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity   = NOPARITY;

    if (!SetCommState(hSerial, &dcbSerialParams)) {
        std::cerr << "Blad ustawiania parametrow portu.\n";
        CloseHandle(hSerial);
        return 1;
    }

    // 3. Konfiguracja Timeouts (bardzo krótkie dla p³ynnoœci)
    COMMTIMEOUTS timeouts = {0};
    timeouts.ReadIntervalTimeout = 2;
    timeouts.ReadTotalTimeoutConstant = 5;
    timeouts.ReadTotalTimeoutMultiplier = 1;
    SetCommTimeouts(hSerial, &timeouts);

    // 4. Zmienne robocze
    uint8_t tx_buffer[20] = {0};
    uint8_t rx_byte = 0;
    DWORD bytesWritten, bytesRead;

    // Przyk³adowa prêdkoœæ symulowana w pêtli
    int predkosc = 260;

    std::cout << "Rozpoczynam wysylanie ramek 50Hz na porcie " << portName << "...\n";

    // Zmienna do kontrolowania pêtli 50 Hz (20 ms)
    auto next_frame = std::chrono::steady_clock::now();

    // 5. G³ówna pêtla wysy³aj¹ca
    while (true) {
        // --- KROK A: Budowa ramki ---
        // Format: [0: SOF 255] [1: Predkosc Setki] [2: Predkosc Reszta] [3-18: Inne dane] [19: Suma Kontrolna]

        // Zabezpieczenie przed przekroczeniem limitów (przyk³ad do max 25499)
        int bezpieczna_predkosc = (predkosc < 0) ? 0 : predkosc;

        tx_buffer[0] = 255; // Znacznik pocz¹tku ramki (SOF)
        tx_buffer[1] = bezpieczna_predkosc / 100; // Np. 2
        tx_buffer[2] = bezpieczna_predkosc % 100; // Np. 60

        // Reszta bufora (indeksy 3-18) pozostaje wype³niona zerami (mo¿esz tu dodaæ inne dane)

        // Obliczanie sumy kontrolnej (bajty 1-18)
        uint8_t suma = 0;
        for (int i = 1; i < 19; i++) {
            suma += tx_buffer[i];
        }
        // Operacja modulo 250 upewnia nas, ¿e suma kontrolna nigdy nie wyniesie 255
        tx_buffer[19] = suma % 250;

        // --- KROK B: Wys³anie ramki ---
        WriteFile(hSerial, tx_buffer, 20, &bytesWritten, NULL);

        // --- KROK C: Odbiór potwierdzenia z Arduino ---
        if (ReadFile(hSerial, &rx_byte, 1, &bytesRead, NULL) && bytesRead == 1) {
            if (rx_byte == 0x00) {
                // Potwierdzenie odebrane prawid³owo (odkomentuj poni¿ej, by widzieæ w konsoli)
                // std::cout << "Arduino potwierdzilo odbior ramki.\n";
            }
        }

        // --- KROK D: Utrzymanie równego taktu 50 Hz ---
        next_frame += std::chrono::milliseconds(20);
        std::this_thread::sleep_until(next_frame);
    }

    CloseHandle(hSerial);
    return 0;
}
