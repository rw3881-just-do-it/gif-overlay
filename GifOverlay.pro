QT       += core gui widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

TARGET = GifOverlay
TEMPLATE = app

SOURCES += \
    main.cpp \
    GifOverlay.cpp

HEADERS += \
    GifOverlay.h

RESOURCES += \
    resources.qrc

# Windows: no console window
WIN32: SUBSYSTEM = windows
win32:CONFIG += windows
