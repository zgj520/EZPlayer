import QtQuick 2.12
//import QtGraphicalEffects 1.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.2
import GUIKit 1.0

Window {
	id:root
	flags: Qt.Window | Qt.FramelessWindowHint | Qt.WindowMinMaxButtonsHint | Qt.WindowCloseButtonHint | Qt.WindowSystemMenuHint
    visible: true
    title: "EZPlayer"
	width: 1280
	height: 720
    property bool isFullScreen: false
    property var playerWindow1: null
    onPlayerWindow1Changed: {
        root.playerWindow1.x = Qt.binding(function (){return playerContainer1.x})
        root.playerWindow1.y = Qt.binding(function (){return playerContainer1.y})
        root.playerWindow1.width = Qt.binding(function (){return playerContainer1.width})
        root.playerWindow1.height = Qt.binding(function (){return playerContainer1.height})
        root.playerWindow1.visible = Qt.binding(function (){return playerContainer1.visible})
    }
    property var playerWindow2: null
    onPlayerWindow2Changed: {
        root.playerWindow2.x = Qt.binding(function (){return playerContainer2.x})
        root.playerWindow2.y = Qt.binding(function (){return playerContainer2.y})
        root.playerWindow2.width = Qt.binding(function (){return playerContainer2.width})
        root.playerWindow2.height = Qt.binding(function (){return playerContainer2.height})
        root.playerWindow2.visible = Qt.binding(function (){return playerContainer2.visible})
    }
    property var playerWindow3: null
    onPlayerWindow3Changed: {
        root.playerWindow3.x = Qt.binding(function (){return playerContainer3.x})
        root.playerWindow3.y = Qt.binding(function (){return playerContainer3.y})
        root.playerWindow3.width = Qt.binding(function (){return playerContainer3.width})
        root.playerWindow3.height = Qt.binding(function (){return playerContainer3.height})
        root.playerWindow3.visible = Qt.binding(function (){return playerContainer3.visible})
    }
    property var playerWindow4: null
    onPlayerWindow4Changed: {
        root.playerWindow4.x = Qt.binding(function (){return playerContainer4.x})
        root.playerWindow4.y = Qt.binding(function (){return playerContainer4.y})
        root.playerWindow4.width = Qt.binding(function (){return playerContainer4.width})
        root.playerWindow4.height = Qt.binding(function (){return playerContainer4.height})
        root.playerWindow4.visible = Qt.binding(function (){return playerContainer4.visible})
    }

	Rectangle{
		id: bkRc
        anchors.fill: parent
        color: GUIColor.Yellow0
        border.color: GUIColor.Cyan2
        border.width: 0.5
        MouseArea{
            anchors.fill: titleBar
            acceptedButtons: Qt.LeftButton
            property point clickPos: "0,0"
            onPressed: {
                clickPos = Qt.point(mouse.x, mouse.y)
            }
            onPositionChanged: {
                if(root.isFullScreen){
                    return
                }

                var delta = Qt.point(mouse.x - clickPos.x, mouse.y - clickPos.y)
                root.setX(root.x + delta.x)
                root.setY(root.y + delta.y)
            }
            onDoubleClicked: {
                if(root.isFullScreen){
                    root.showNormal()
                }
                else{
                    root.showFullScreen()
                }
                root.isFullScreen = !root.isFullScreen
            }
        }
        Connections{
            target: titleBar
            onSignalReqClose: {
                root.close()
            }
        }
        TitleBar{
            id: titleBar
            anchors{
                left: parent.left
                leftMargin: 1
                top: parent.top
                topMargin: 1
                right: parent.right
                rightMargin: 1
            }
        }
        Rectangle{
            id: playerContainer1
             anchors{
                    left: parent.left
                    leftMargin: 1
                    top: titleBar.bottom
                    right: parent.right
                    rightMargin: parent.width/2
                    bottom:parent.bottom
                    bottomMargin: parent.height/2
             }
        }
        Rectangle{
            id: playerContainer2
             anchors{
                    left: playerContainer1.right
                    leftMargin: 0
                    top: titleBar.bottom
                    right: parent.right
                    rightMargin: 1
                    bottom:playerContainer1.bottom
                    bottomMargin: 0
             }
        }
        Rectangle{
            id: playerContainer3
             anchors{
                    left: playerContainer1.left
                    top: playerContainer1.bottom
                    right: playerContainer1.right
                    bottom:parent.bottom
                    bottomMargin: 1
             }
        }
        Rectangle{
            id: playerContainer4
            anchors{
                    left: playerContainer2.left
                    top: playerContainer2.bottom
                    right: playerContainer2.right
                    bottom:parent.bottom
                    bottomMargin: 1
             }
        }
	}
}
