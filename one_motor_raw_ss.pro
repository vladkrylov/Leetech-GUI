#-------------------------------------------------
#
# Project created by QtCreator 2013-12-13T14:01:35
#
#-------------------------------------------------

QT       += widgets
QT       += core gui
QT       += network

TARGET = one_motor_raw
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    ip_connection.cpp \
    encoder.cpp \
    controller.cpp \
    tests.cpp

HEADERS  += mainwindow.h \
    ip_connection.h \
    encoder.h \
    controller.h \
    tests.h

FORMS    += mainwindow.ui
