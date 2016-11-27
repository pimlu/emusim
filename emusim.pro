QT += core
QT += widgets
#QT -= gui

CONFIG += c++11

TARGET = emusim
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    ui/mainwindow.cpp

HEADERS += \
    ui/mainwindow.h
