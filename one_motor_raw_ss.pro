#-------------------------------------------------
#
# Project created by QtCreator 2013-12-13T14:01:35
#
#-------------------------------------------------

QT       += core gui
QT       += network

TARGET = one_motor_raw
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    ip_connection.cpp \
    encoder.cpp

HEADERS  += mainwindow.h \
    ip_connection.h \
    encoder.h

FORMS    += mainwindow.ui
