import QtQuick 2.4

Rectangle{

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
}
