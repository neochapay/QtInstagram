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

    Rectangle{
        property string text

        id: failAction
        width: parent.width
        height: 50
        color: "#aa155c"

        z: 2

        visible: false

        anchors{
            top: parent.top
            left: parent.left
        }

        Text{
            id: failText
            color: "white"
            width: parent.width
            text: failAction.text

            horizontalAlignment: Text.AlignHCenter

            anchors{
                verticalCenter: parent.verticalCenter
            }
        }

        MouseArea{
            anchors.fill: parent
            onClicked: {
                failAction.visible = false
                failAction.text = "";
            }
        }
    }

    Loader{
        id: loader
        width: parent.width
        height: parent.height

        z: 1
    }

    Component.onCompleted: {
        loader.source = "pages/FrontPage.qml"
    }

    Connections{
        target: instagram
        onError:{
            failAction.text = message
            failAction.visible = true;
        }
    }
}
