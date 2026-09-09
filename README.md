# ForZero

Sterownik fizycznego zestawu wskaźników Audi A4 B6,
pozwalający wyświetlać telemetrię z Forza Horizon 6
przy użyciu STM32, CAN, USB i autorskiej aplikacji Qt.



ForZero powstał jako próba wykorzystania fizycznego zestawu wskaźników Audi A4 B6 jako elementu stanowiska simracingowego. Licznik wykorzystuje kilka różnych sposobów sterowania: magistralę CAN, sygnał impulsowy, wejścia rezystancyjne oraz sygnały 12 V.

Dane telemetryczne z gry są odbierane przez autorską aplikację Qt, przetwarzane i wysyłane przez USB VCP do sterownika opartego na STM32F105. Sterownik generuje następnie sygnały wymagane przez licznik.

## Demo

▶️ Film przedstawiający projekt: [YouTube](...)