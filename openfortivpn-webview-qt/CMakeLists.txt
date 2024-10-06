cmake_minimum_required(VERSION 3.16)

project(openfortivpn-webview VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets WebEngineWidgets)
qt_standard_project_setup()

qt_add_executable(openfortivpn-webview
    main.cpp
    mainwindow.cpp mainwindow.h
)

target_compile_definitions(openfortivpn-webview PRIVATE
    QT_DEPRECATED_WARNINGS
    QT_DISABLE_DEPRECATED_BEFORE=0x060000
)

set_target_properties(openfortivpn-webview PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
)

target_link_libraries(openfortivpn-webview PRIVATE
    Qt::Core
    Qt::Gui
    Qt::WebEngineWidgets
    Qt::Widgets
)
