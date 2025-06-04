QT       += core core-private

# Check if platformsupport-private is available
qtHaveModule(platformsupport-private) {
    QT += platformsupport-private
} else {
    # For newer Qt versions, try gui-private instead
    QT += gui-private
    DEFINES += NO_PLATFORMSUPPORT_PRIVATE
}

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
CONFIG += link_pkgconfig
PKGCONFIG += glib-2.0

TEMPLATE = lib
TARGET = qtasio

# This makes the .so file's version match the project version
VERSION = 1.0.0

# We want the library to be installed into the Qt plugins directory
target.path = $$[QT_INSTALL_LIBS]
INSTALLS += target

# Define to create a shared library
DEFINES += QTASIO_LIBRARY

LIBS += -lboost_system

# Include directory
INCLUDEPATH += ../include

SOURCES += \
    qasioeventdispatcher.cpp

HEADERS += \
    ../include/qtasio/qasioeventdispatcher.h

# Header installation
headers.files = ../include/qtasio/*.h
headers.path = $$[QT_INSTALL_HEADERS]/qtasio
INSTALLS += headers