#include <SFML/Graphics.hpp>
#include <optional>

int main() {
    sf::RenderWindow window(sf::VideoMode({800, 600}), "For Zero: Driver", sf::Style::Default);
    window.setFramerateLimit(60);
    // Zapisanie domyślnego widoku zaraz po stworzeniu okna
    sf::View view = window.getDefaultView();

    // 1. Inicjalizacja i ładowanie czcionki
            sf::Font font;
            
            // W SFML 3 używamy metody openFromFile do zasobów
            if (!font.openFromFile("C:/Windows/Fonts/arial.ttf")) {
                return -1; // Zatrzymanie programu, jeśli brakuje czcionki
            }

            // 2. Tworzenie obiektu tekstu
            // SFML 3 wymaga przekazania czcionki w konstruktorze
            sf::Text text(font, "forZero - Oczekiwanie na telemetrie...", 30);
            text.setFillColor(sf::Color::White);
            
            // Ustawienie napisu na środku bazowego obszaru 800x600
            text.setPosition({150.f, 280.f});

    while (window.isOpen()) {
        // SFML 3: pollEvent zwraca std::optional<sf::Event>
        while (const std::optional event = window.pollEvent()) {
            
            // Sprawdzanie zdarzeń nieprzenoszących danych (np. zamknięcie okna) przez ->is<T>()
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }

            

            if (const auto* resized = event->getIf<sf::Event::Resized>()) {
                float windowWidth = static_cast<float>(resized->size.x);
                float windowHeight = static_cast<float>(resized->size.y);
                
                // Zdefiniuj bazowe proporcje Twojej aplikacji (tutaj z okna 800x600)
                float viewRatio = 800.f / 600.f;
                float windowRatio = windowWidth / windowHeight;

                float sizeX = 1.f;
                float sizeY = 1.f;
                float posX = 0.f;
                float posY = 0.f;

                bool isWindowNarrower = windowRatio < viewRatio;

                // Jeśli okno jest węższe niż proporcje bazowe (dodajemy paski na górze i dole)
                if (isWindowNarrower) {
                    sizeY = windowRatio / viewRatio;
                    posY = (1.f - sizeY) / 2.f;
                } 
                // Jeśli okno jest szersze (dodajemy paski po bokach)
                else {
                    sizeX = viewRatio / windowRatio;
                    posX = (1.f - sizeX) / 2.f;
                }

                // SFML 3: Aktualizujemy obszar docelowy rysowania na oknie (Viewport)
                // Wartości podawane są w zakresie 0.0 do 1.0 (procent wymiarów okna)
                view.setViewport(sf::FloatRect({posX, posY}, {sizeX, sizeY}));
                window.setView(view);
            }
        }

        window.clear(sf::Color::Black);
        
        // Twoja logika rysowania mapy i obiektów
        // window.draw(...);
        
        // 3. Rysowanie tekstu na ekranie
        window.draw(text);

        window.display();
    }

    return 0;
}