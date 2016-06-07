import QtQuick 2.0

import "../js/FontAwesome.js" as FontAwesome

Rectangle {
    id: timeLineItem

    height: childrenRect.height+18
    width: parent.width

    Image{
        id: itemImage
        width: parent.width
        height: (parent.width)/image_versions2.candidates[0].width*image_versions2.candidates[0].height
        source: image_versions2.candidates[0].url
        anchors{
            top: parent.top
            topMargin: 9
            left: parent.left
        }
    }

    Rectangle{
        height: childrenRect.height
        width: parent.width-18
        anchors{
            top: itemImage.bottom
            left: parent.left
        }

        Text{
            id: likeIcon
            width: 20
            height: 20
            anchors{
                left: parent.left
                leftMargin: 4
                top: parent.top
                topMargin: 4
            }
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter

            text: (has_liked)? FontAwesome.Icon.heart : FontAwesome.Icon.heart_o
            color: "#5caa15"
            font{
                family: "FontAwesome"
                pixelSize: 18
            }

            MouseArea{
                anchors.fill: parent
                onClicked: {
                    timeLineView.currentIndex = index;

                    if(has_liked)
                    {
                        instagram.unLike(id)
                    }
                    else
                    {
                        instagram.like(id)
                    }
                }
            }
        }

        Text{
            text: caption.text
            anchors{
                top: likeIcon.top
                left: likeIcon.right
                leftMargin: 4
            }
        }
    }

    Connections{
        target: instagram
        onLikeDataReady:{
            var out = JSON.parse(answer)
            if(out.status === "ok")
            {
                timeLineView.model.get(timeLineView.currentIndex).has_liked = true
            }
        }
    }

    Connections{
        target: instagram
        onUnLikeDataReady:{
            var out = JSON.parse(answer)
            if(out.status === "ok")
            {
                timeLineView.model.get(timeLineView.currentIndex).has_liked =  false
            }
        }
    }
}
