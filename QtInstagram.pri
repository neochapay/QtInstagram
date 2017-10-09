QT += \
    gui \
    network

# Use API v1 only if explicitly requested
equals(INSTAGRAM_API_VERSION, "v1") {
    BASE_DIR = $${PWD}/src/api
    SOURCES += \
        $${BASE_DIR}/instagramrequest.cpp \
        $${BASE_DIR}/instagram.cpp
    HEADERS += \
        $${BASE_DIR}/instagramrequest.h \
        $${BASE_DIR}/instagram.h
} else {
    BASE_DIR = $${PWD}/src/api2
    SOURCES += \
        $${BASE_DIR}/instagramconstants.cpp \
        $${BASE_DIR}/instagramrequestv2.cpp \
        $${BASE_DIR}/instagramv2.cpp \
        $${BASE_DIR}/request/account.cpp \
        $${BASE_DIR}/request/direct.cpp \
        $${BASE_DIR}/request/discover.cpp \
        $${BASE_DIR}/request/hashtag.cpp \
        $${BASE_DIR}/request/media.cpp \
        $${BASE_DIR}/request/people.cpp \
        $${BASE_DIR}/request/story.cpp \
        $${BASE_DIR}/request/timeline.cpp \
        $${BASE_DIR}/request/usertag.cpp
    HEADERS += \
        $${BASE_DIR}/instagramconstants.h \
        $${BASE_DIR}/instagramrequestv2.h \
        $${BASE_DIR}/instagramv2.h
}

SOURCES += \
    $${PWD}/src/cripto/hmacsha.cpp

HEADERS += \
    $${PWD}/src/cripto/hmacsha.h

INCLUDEPATH += \
    $${BASE_DIR}
