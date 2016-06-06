import QtQuick 2.4
import QtQuick.Window 2.2

Window {
    visible: true

    Image{
        id: backImage
        width: parent.width
        height: parent.height

        source: "img/prosto_back.jpg"

        fillMode: Image.PreserveAspectCrop

        clip: true
    }

    Image {
        id: logoText
        source: "img/name1024.png"

        width: parent.width-0.25*parent.width
        height: 0.20*parent.height

        fillMode: Image.PreserveAspectFit

        anchors{
            top: parent.top
            topMargin: 0.125*parent.width
            left: parent.left
            leftMargin: 0.125*parent.width
        }
    }

    Rectangle{
        id: entherAction
        color: "#5caa15"
        clip: true
        radius: 5

        width: parent.width-0.25*parent.width
        height: 0.20*parent.height

        anchors{
            top: logoText.bottom
            topMargin: 0.25*parent.width
            left: parent.left
            leftMargin: 0.125*parent.width
        }

        Rectangle{
            id: loginButton
            width: parent.width
            height: parent.height/2

            color: "transparent"

            anchors{
                top: parent.top
                left: parent.left
            }

            Text{
                text: qsTr("Login")
                color: "white"
                width: parent.width
                height: parent.height-2

                fontSizeMode: Text.Fit
                minimumPixelSize: 10
                font.pixelSize: 72

                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }

        Rectangle{
            id: regButton
            width: parent.width
            height: parent.height/2
            radius: 5

            color: "#155caa"

            anchors{
                top: loginButton.bottom
                left: parent.left
            }

            Text{
                text: qsTr("Registration")
                color: "white"
                width: parent.width
                height: parent.height-2

                fontSizeMode: Text.Fit
                minimumPixelSize: 10
                font.pixelSize: 72

                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }

            Rectangle{
                color: parent.color
                width: parent.radius
                height: parent.radius
                anchors{
                    top: parent.top
                    left: parent.left
                }
            }

            Rectangle{
                color: parent.color
                width: parent.radius
                height: parent.radius
                anchors{
                    top: parent.top
                    right: parent.right
                }
            }
        }
    }
}
