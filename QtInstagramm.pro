TEMPLATE = lib

VERSION = 0.1

QT += network

SOURCES += \
    src/api/instagram.cpp \
    src/api/instagramrequest.cpp \
    src/cripto/hmacsha.cpp

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    src/api/instagram.h \
    src/api/instagramrequest.h \
    src/cripto/hmacsha.h
