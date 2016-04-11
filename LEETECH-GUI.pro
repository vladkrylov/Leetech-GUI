#-------------------------------------------------
#
# Project created by QtCreator 2013-12-13T14:01:35
#
#-------------------------------------------------

QT       += widgets
QT       += core gui
QT       += network
QT       += testlib
QT       += serialport

TARGET = one_motor_raw
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    ip_connection.cpp \
    controller.cpp \
    tests.cpp \
    encoder.cpp \
    collimatorsset.cpp \
    trajectory.cpp \
    collimatorgraphicsitem.cpp

HEADERS  += mainwindow.h \
    ip_connection.h \
    controller.h \
    tests.h \
    encoder.h \
    collimatorsset.h \
    trajectory.h \
    m_errors.h \
    collimatorgraphicsitem.h \
    types.h

FORMS    += mainwindow.ui

OTHER_FILES += \
    LEETECH-GUI.pro.user \
    one_motor_raw_ss.pro.user
