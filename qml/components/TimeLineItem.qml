import QtQuick 2.0

import "../js/FontAwesome.js" as FontAwesome

Rectangle {
    id: timeLineItem

    height: childrenRect.height+18
    width: parent.width

    Component.onCompleted: {
        var date = new Date(taken_at*1000);
        var months = [qsTr('Jan'),
                      qsTr('Feb'),
                      qsTr('Mar'),
                      qsTr('Apr'),
                      qsTr('May'),
                      qsTr('Jun'),
                      qsTr('Jul'),
                      qsTr('Aug'),
                      qsTr('Sep'),
                      qsTr('Oct'),
                      qsTr('Nov'),
                      qsTr('Dec')];
        var hours;
        var minutes;
        if(date.getHours() < 10)
        {
            hours = "0"+date.getHours();
        }
        else
        {
            hours = date.getHours();
        }
        if(date.getMinutes() < 10)
        {
            minutes = "0"+date.getMinutes();
        }
        else
        {
            minutes = date.getMinutes();
        }

        itemTime.text = date.getDate()+" "+months[date.getMonth()]+" "+hours+":"+minutes
    }

    Rectangle{
        id: infoLine
        height: 28
        width: parent.width

        Image{
            id: profilePic
            height: 20
            width: 20
            source: user.profile_pic_url

            anchors{
                left: parent.left
                leftMargin: 4
                top: parent.top
                topMargin: 4
            }
        }

        Text {
            id: nickName
            text: user.username

            height: 20

            anchors{
                left: profilePic.right
                leftMargin: 4
                top: parent.top
                topMargin: 4
            }

            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter

            color: "#5caa15"
        }

        Text{
            id: itemTime
            height: 20

            anchors{
                right: parent.right
                rightMargin: 4
                top: parent.top
                topMargin: 4
            }

            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
        }
    }

    Image{
        id: itemImage
        width: parent.width
        height: (parent.width)/image_versions2.candidates[0].width*image_versions2.candidates[0].height
        source: image_versions2.candidates[0].url
        anchors{
            top: infoLine.bottom
            left: parent.left
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

    Rectangle{
        id: commentsLikes
        height: childrenRect.height
        width: parent.width
        anchors{
            top: itemImage.bottom
            topMargin: 4
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

    Rectangle{
        id: commentLine
        width: parent.width-8
        height: childrenRect.height+4

        anchors{
            left: parent.left
            leftMargin: 4
            top: commentsLikes.bottom
            topMargin: 4
        }

        ListView{
            id: commentView
            width: parent.width
            height: childrenRect.height
            model: comments

            delegate: Rectangle{
                width: parent.width
                height: childrenRect.height

                Component.onCompleted: {
                    commentText.text = '<b>'+user.username+"</b> : "+text
                }

                Text{
                    id: commentText
                }
            }
        }

        Text{
            id: andMoreComments
            visible: (comment_count > commentView.count) ? true : false
            text: "More comments..."

            anchors{
                top: commentView.bottom
                left: commentView.left
            }
        }
    }

    Rectangle{
        id: doCommentLine
        height: 28
        width: parent.width

        anchors{
            left: parent.left
            top: commentLine.bottom
        }

        SimpleTextFeed{
            id: commentArea
            width: parent.width-sendCommentIcon.width-8
            height: parent.height-8

            placeholder: qsTr("Add comment")

            anchors{
                top: parent.top
                topMargin: 4
                left: parent.left
                leftMargin: 4
            }
        }

        Text{
            id: sendCommentIcon
            width: 20
            height: 20
            anchors{
                right: parent.right
                rightMargin: 4
                top: parent.top
                topMargin: 4
            }
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter

            text: FontAwesome.Icon.comment_o
            color: "#5caa15"
            font{
                family: "FontAwesome"
                pixelSize: 18
            }

            MouseArea{
                anchors.fill: parent
                onClicked: {
                    if(commentArea.text.length > 0)
                    {
                        instagram.postComment(id,commentArea.text)
                    }
                }
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

    Connections{
        target: instagram
        onCommentPosted:{
            commentArea.setText("");
            var comment = JSON.parse(answer)
            comments.append(comment.comment)
        }
    }
}
