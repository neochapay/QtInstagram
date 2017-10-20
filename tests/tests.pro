TEMPLATE = app
TARGET = tst_instagram

QT += qml testlib

SRC_DIR = $${PWD}/../src/api2

INCLUDEPATH += \
    $${SRC_DIR}
LIBS += \
    -L../src -lQtInstagram
QMAKE_RPATHDIR = ../src

SOURCES += \
    tst_instagram.cpp

HEADERS += \
    fake_network.h

check.commands = ./$${TARGET}
check.depends = $${TARGET}
QMAKE_EXTRA_TARGETS += check
