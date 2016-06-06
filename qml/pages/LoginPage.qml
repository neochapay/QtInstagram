import QtQuick 2.5
import QtQuick.Controls 1.5
import QtQuick.Controls.Styles 1.4

import "../components/"

Rectangle{
    id: loginPage
    Image{
        id: backImage
        width: parent.width
        height: parent.height

        source: "../../img/prosto_back.jpg"

        fillMode: Image.PreserveAspectCrop

        clip: true
    }

    Image {
        id: logoText
        source: "../../img/name1024.png"

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
        color: "white"
        clip: true
        radius: 5

        width: parent.width-0.25*parent.width
        height: 0.30*parent.height

        anchors{
            top: logoText.bottom
            topMargin: 0.25*parent.width
            left: parent.left
            leftMargin: 0.125*parent.width
        }

        SimpleTextFeed{
            id: loginArea
            width: parent.width-10
            height: loginButton.height

            placeholder: qsTr("Login")

            anchors{
                top: parent.top
                left: parent.left
                leftMargin: 5
            }
        }

        SimpleTextFeed{
            id: passwordArea
            width: parent.width-10
            height: loginButton.height

            placeholder: qsTr("Password")
            echo: TextInput.Password

            anchors{
                top: loginArea.bottom
                left: parent.left
                leftMargin: 5
            }
        }

        Rectangle{
            id: loginButton
            width: parent.width
            height: parent.height/3
            radius: 5

            color: "#5caa15"

            anchors{
                bottom: parent.bottom
                left: parent.left
            }

            Text{
                text: qsTr("Login")
                color: "white"
                width: parent.width
                height: parent.height/3*2

                fontSizeMode: Text.Fit
                minimumPixelSize: 10
                font.pixelSize: 72

                anchors{
                    verticalCenter: parent.verticalCenter
                }

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
