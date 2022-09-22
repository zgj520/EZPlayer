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
        Player{
            id: playerContainer1
            parentWindow: root
             width: 400
             height: 225
             x:1
             y:titleBar.height + titleBar.y
        }
	}
}
