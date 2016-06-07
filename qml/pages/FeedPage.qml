import QtQuick 2.0
import "../components/"

Rectangle {
    id: feedPage

    Component.onCompleted: {
        instagram.getTimeLine();
    }

    ListModel{
        id: timeLineItemsModel
    }

    Connections{
        target: instagram
        onTimeLineDataReady: {
            var timeline = JSON.parse(answer);
            timeLineItemsModel.clear();
            if(timeline.num_results > 0)
            {
                timeline.items.forEach(function(item){
                    timeLineItemsModel.append(item);
                })
            }
        }
    }

    TimeLineToolbar{
        id: toolBar
        anchors{
            top: parent.top
            left: parent.left
        }
    }

    ListView{
        id: timeLineView

        width: parent.width
        height: parent.height-toolBar.height

        model: timeLineItemsModel

        clip: true

        anchors{
            top: toolBar.bottom
            left: parent.left
        }

        delegate: TimeLineItem{}
    }
}
