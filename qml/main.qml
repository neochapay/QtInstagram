import QtQuick 2.4
import QtQuick.Window 2.2

import "pages"

Window {
    id: main
    visible: true

    Loader{
        id: loader
        width: parent.width
        height: parent.height
    }

    Component.onCompleted: {
        loader.source = "pages/FrontPage.qml"
    }
}
