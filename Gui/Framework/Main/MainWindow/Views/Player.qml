import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.2
import PlayerViewModel 1.0
import PlayerWindow 1.0

Rectangle {
	id:root
    width: 400
    height: 400
    property var parentWindow: null
    property PlayerViewModel viewModel: PlayerViewModel{}
    property var playRenderWindow: null
    color: Qt.rgba(0,1,0,1)
    radius: 4

    Rectangle{
        id: title
        color:  Qt.rgba(45/255,45/255,48/255,1)
        property bool isFullScreen: false
        anchors{
            left: parent.left
            right: parent.right
            top:parent.top
        }
        height: 40
        MouseArea{
            anchors.fill: parent
            property point clickPos: "0,0"
            onPressed: {
                clickPos = Qt.point(mouse.x, mouse.y)
            }
            onPositionChanged: {
                var delta = Qt.point(mouse.x - clickPos.x, mouse.y - clickPos.y)

                root.x = (root.x + delta.x)
                root.y = (root.y + delta.y)
                console.log(delta, root.x, root.y)
            }
            onDoubleClicked: {
                root.isFullScreen = !root.isFullScreen
            }
        }
    }
    Rectangle{
        id: body
        color:  Qt.rgba(0,0,1,1)
        anchors{
            left: parent.left
            right: parent.right
            top:title.bottom
            bottom: parent.bottom
        }
        DropArea{
            anchors.fill: parent
            onEntered: {
                drag.accepted = true
            }

            onDropped: {
                viewModel.dropEventDeal(drop.urls[0].toString())
                console.log( drop.urls[0].toString())
            }
        }
    }


    Component.onCompleted: {
        viewModel.createPlayerRenderWindow(parentWindow)
        root.playRenderWindow = viewModel.playerRenderWindow
        root.playRenderWindow.x = Qt.binding(function (){return root.x})
        root.playRenderWindow.y = Qt.binding(function (){return root.y + title.height})
        root.playRenderWindow.width = Qt.binding(function (){return body.width})
        root.playRenderWindow.height = Qt.binding(function (){return body.height})
        root.playRenderWindow.visible = false
    }
}
