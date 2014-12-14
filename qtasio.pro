#-------------------------------------------------
#
# Project created by QtCreator 2014-11-08T21:15:59
#
#-------------------------------------------------

QT       += core core-private platformsupport-private

#only for tests
QT       += gui testlib network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
CONFIG += link_pkgconfig
CONFIG += testcase
PKGCONFIG += glib-2.0

LIBS += -lboost_system

TARGET = qtasio
TEMPLATE = app


SOURCES += \
    qasioeventdispatcher.cpp \
    testqasioeventdispatcher.cpp

HEADERS  += \
    qasioeventdispatcher.h \
    testqasioeventdispatcher.h

FORMS    +=
