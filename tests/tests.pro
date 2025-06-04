QT       += core core-private gui testlib network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

# Check if platformsupport-private is available
qtHaveModule(platformsupport-private) {
    QT += platformsupport-private
} else {
    # For newer Qt versions, try gui-private instead
    QT += gui-private
    DEFINES += NO_PLATFORMSUPPORT_PRIVATE
}

CONFIG += c++11
CONFIG += testcase
CONFIG += link_pkgconfig
PKGCONFIG += glib-2.0

TARGET = qtasio_test
TEMPLATE = app

# Link with our library
LIBS += -L../src -lqtasio -lboost_system

# Include directories
INCLUDEPATH += ../include

SOURCES += \
    testqasioeventdispatcher.cpp

HEADERS += \
    testqasioeventdispatcher.h