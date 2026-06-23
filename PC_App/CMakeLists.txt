cmake_minimum_required(VERSION 3.14)

# DODANE: Ignorowanie błędu certyfikatów SSL przy pobieraniu bibliotek z GitHuba
set(CMAKE_TLS_VERIFY OFF)

project(FOR_ZERO_PC_App VERSION 1.0)

# KRYTYCZNE DLA SFML 3: Wymuszenie standardu przynajmniej C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Budowanie bibliotek jako statyczne (brak problemów z plikami .dll)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build static SFML" FORCE)

# Pobieranie SFML
include(FetchContent)
FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.1.0  
)
FetchContent_MakeAvailable(SFML)

add_executable(${PROJECT_NAME} src/main.cpp)

# Podpinanie modułu (w SFML 3 preferowany jest zapis z przestrzenią nazw SFML::)
target_link_libraries(${PROJECT_NAME} PRIVATE SFML::Graphics)