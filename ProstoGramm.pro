TEMPLATE = app

QT += qml quick
CONFIG += c++11

SOURCES += main.cpp \
    src/api/instagram.cpp \
    src/api/instagramrequest.cpp \
    src/cripto/hmacsha.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    src/api/instagram.h \
    src/api/instagramrequest.h \
    src/cripto/hmacsha.h

DISTFILES += \
    img/prosto_back.jpg \
    img/name1024.png
