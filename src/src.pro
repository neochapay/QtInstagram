TEMPLATE = lib
TARGET = QtInstagram

VERSION = 0.1

include(../QtInstagram.pri)

CONFIG(coverage) {
    QMAKE_CXXFLAGS += -fprofile-arcs -ftest-coverage
    LIBS += -lgcov
}
