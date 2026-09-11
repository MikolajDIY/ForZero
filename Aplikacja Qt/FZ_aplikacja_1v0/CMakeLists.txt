cmake_minimum_required(VERSION 3.16)

project(Fz_udp VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS
    Quick
    Network
    SerialPort
)

qt_standard_project_setup(REQUIRES 6.8)

# 1. Najpierw tworzymy target wykonywalny
qt_add_executable(appFz_udp
    main.cpp
    resources.rc
)

# 2. Dopiero po utworzeniu targetu dodajemy moduł QML i pliki C++
qt_add_qml_module(appFz_udp
    URI Fz_udp
    QML_FILES
        Main.qml
    SOURCES
        fh6_telemetryreciver.h
        fh6_telemetryreciver.cpp
        fh6_cluster_transciver.h
        fh6_cluster_transciver.cpp
        audi_a4_b6_cluster.h
        audi_a4_b6_cluster.cpp
        SOURCES utils.h
        SOURCES usb_worker.h
        SOURCES cluster.h
        SOURCES usb_worker.cpp
        SOURCES
        SOURCES cluster_settings.h
        SOURCES latency_tracker.h
)

# 3. Podlinkowanie bibliotek Qt
target_link_libraries(appFz_udp
    PRIVATE
        Qt6::Quick
        Qt6::Network
        Qt6::SerialPort
)

# 4. Właściwości targetu
set_target_properties(appFz_udp PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

# 5. Instalacja
include(GNUInstallDirs)
install(TARGETS appFz_udp
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
