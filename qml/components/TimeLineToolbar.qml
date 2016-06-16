import QtQuick 2.0

Rectangle {
    width: parent.width
    height: 50

    color: "#EEEEEE"

    Image{
        id: icon
        width: 32
        height: 32

        source: "../../img/icon64.png"

        anchors{
            left: parent.left
            leftMargin: 9
            top: parent.top
            topMargin: 9
        }

        MouseArea{
            anchors.fill: parent
            onClicked: {
                instagram.postImage("/home/neochapay/1.jpg","tst");
            }
        }
    }

    Text{
        id: nickText
        height: parent.height
        width: parent.width-100

        text: main.profile.logged_in_user.username

        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter

        anchors{
            left: icon.right
            leftMargin: 9
        }
    }

    Image{
        id: profilePic
        width: 32
        height: 32

        source: main.profile.logged_in_user.profile_pic_url

        anchors{
            left: nickText.right
            leftMargin: 9
            top: parent.top
            topMargin: 9
        }
    }
}
