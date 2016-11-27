QT += core
QT += widgets
#QT -= gui

CONFIG += c++11

TARGET = emusim
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    gui/mainwindow.cpp \
    sim/scheduler.cpp \
    sim/process.cpp \
    sim/system.cpp

HEADERS += \
    gui/mainwindow.h \
    sim/scheduler.h \
    sim/process.h \
    sim/system.h \
    sim/syscalls.h
