import QtQuick 2.4
import QtQuick.Window 2.2

import instagram 1.0

import "pages"

Window {
    id: main
    visible: true

    Instagram{
        id: instagram
    }

    Loader{
        id: loader
        width: parent.width
        height: parent.height
    }

    Component.onCompleted: {
        loader.source = "pages/FrontPage.qml"
    }
}
