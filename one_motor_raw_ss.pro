#-------------------------------------------------
#
# Project created by QtCreator 2013-12-13T14:01:35
#
#-------------------------------------------------

QT       += widgets
QT       += core gui
QT       += network
QT       += testlib

TARGET = one_motor_raw
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    ip_connection.cpp \
    controller.cpp \
    tests.cpp \
    encoder.cpp \
    collimatorsset.cpp \
    trajectory.cpp

HEADERS  += mainwindow.h \
    ip_connection.h \
    controller.h \
    tests.h \
    encoder.h \
    collimatorsset.h \
    trajectory.h

FORMS    += mainwindow.ui
