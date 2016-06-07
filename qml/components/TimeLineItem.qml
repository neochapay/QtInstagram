import QtQuick 2.0

Rectangle {
    height: childrenRect.height+18
    width: parent.width

    Image{
        id: itemImage
        width: parent.width-18
        height: (parent.width-18)/image_versions2.candidates[0].width*image_versions2.candidates[0].height
        source: image_versions2.candidates[0].url
        anchors{
            top: parent.top
            topMargin: 9
            left: parent.left
            leftMargin: 9
        }
    }

    Text{
        text: caption.text
        anchors{
            top: itemImage.bottom
            topMargin: 9
            left: parent.left
            leftMargin: 9
        }
    }
}
