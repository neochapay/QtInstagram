import QtQuick 2.5
import QtQuick.Controls 1.5
import QtQuick.Controls.Styles 1.4

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

        Rectangle{
            id: loginArea
            width: parent.width-10
            height: loginButton.height

            anchors{
                top: parent.top
                left: parent.left
                leftMargin: 5
            }

            TextField{
                id: loginInput
                width: parent.width
                height: parent.height

                placeholderText: qsTr("Login")

                anchors{
                    verticalCenter: parent.verticalCenter
                }

                style: TextFieldStyle{
                    background: Rectangle{}
                }
            }
        }

        Rectangle{
            id: passwordArea
            width: parent.width-10
            height: loginButton.height

            anchors{
                top: loginArea.bottom
                left: parent.left
                leftMargin: 5
            }

            TextField{
                id: passwordInput
                width: parent.width
                height: parent.height

                placeholderText: qsTr("Password")

                anchors{
                    verticalCenter: parent.verticalCenter
                }

                echoMode: TextInput.Password

                style: TextFieldStyle{
                    background: Rectangle{}
                }
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
