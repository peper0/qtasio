#-------------------------------------------------
#
# Project created by QtCreator 2014-11-08T21:15:59
#
#-------------------------------------------------

QT       += core core-private

# Check if platformsupport-private is available
qtHaveModule(platformsupport-private) {
    QT += platformsupport-private
} else {
    # For newer Qt versions, try gui-private instead
    QT += gui-private
    DEFINES += NO_PLATFORMSUPPORT_PRIVATE
}

#only for tests
QT       += gui testlib network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
CONFIG += link_pkgconfig
CONFIG += testcase
PKGCONFIG += glib-2.0

LIBS += -lboost_system

TARGET = qtasio
TEMPLATE = subdirs

SUBDIRS += \
    src \
    tests \
    examples

# Dependencies
tests.depends = src
examples.depends = src

# Other project files
OTHER_FILES += \
    README.md \
    LICENSE
