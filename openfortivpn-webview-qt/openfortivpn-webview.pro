QT       += core gui widgets webenginewidgets

TARGET = openfortivpn-webview
TEMPLATE = app

VERSION = "$$cat(version.txt)"
DEFINES += "APPLICATION_VERSION='\"$$VERSION\"'"

DEFINES += QT_DEPRECATED_WARNINGS

DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    mainwindow.h

!lessThan(QT_VERSION, 6.7) {
    SOURCES += webauthdialog.cpp
    HEADERS += webauthdialog.h
    FORMS += webauthdialog.ui
}
