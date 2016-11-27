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
    sim/system.cpp \
    sim/filesystem.cpp \
    emu/emuprocess.cpp \
    gui/systemthread.cpp

HEADERS += \
    gui/mainwindow.h \
    sim/scheduler.h \
    sim/process.h \
    sim/system.h \
    sim/syscalls.h \
    sim/filesystem.h \
    emu/emuprocess.h \
    gui/systemthread.h
