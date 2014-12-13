#-------------------------------------------------
#
# Project created by QtCreator 2014-11-08T21:15:59
#
#-------------------------------------------------

QT       += core gui core-private platformsupport-private

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

#CONFIG += c++11
CONFIG += link_pkgconfig
PKGCONFIG += glib-2.0

LIBS += -lboost_system

TARGET = qtasio
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    qasioeventdispatcher.cpp

HEADERS  += mainwindow.h \
    qasioeventdispatcher.h

FORMS    += mainwindow.ui
