import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.2
import PlayerViewModel 1.0
import PlayerWindow 1.0
import GUIKit 1.0

Rectangle {
	id:root
    color: Qt.rgba(0,1,0,1)
    radius: 4
    signal signalReqClose()
    property var parentWindow: null
    property PlayerViewModel viewModel: PlayerViewModel{}
    property var playRenderWindow: null
    property bool isPlaying: false
    property var wh: 4/3.0
    function dealDropEvent(filePath){
         fileName.text = filePath.replace("file:///", "")
        viewModel.dropEventDeal(filePath)
    }
    height: (width-title.height-playControlRc.height)/root.wh + title.height + playControlRc.height
    MouseArea{
        anchors.fill: parent
        hoverEnabled: true
        onPositionChanged: {
            if(mouseX > width - 8 && mouseY >= height - 8){
                cursorShape = Qt.SizeFDiagCursor
            }
            else{
                cursorShape = Qt.ArrowCursor
            }

            if(mouse.buttons & Qt.LeftButton){
                root.width = mouseX
                //root.height = mouseY
                console.log(width, height, root.wh)
            }
        }
        onExited: {
            cursorShape = Qt.ArrowCursor
        }
    }

    Rectangle{
        id: title
        color:  Qt.rgba(45/255,45/255,48/255,1)
        property bool isFullScreen: false
        anchors{
            left: parent.left
            right: parent.right
            top:parent.top
        }
        height: 32
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
            }
            onDoubleClicked: {

            }
        }

        Label{
            id: fileName
            text: ""
            anchors{
                verticalCenter: parent.verticalCenter
                left: parent.left
                leftMargin: 8
            }
            font: GUIFont.SubtitleSemibold12
            anchors{
               verticalCenter: parent.verticalCenter
               left: parent.left
               leftMargin: 8
            }
            color: GUIColor.White00
        }

        GUIImageButton {
            id: infoBtn
            width: 24
            height: 24
            mouseCursorShape: Qt.PointingHandCursor
            anchors{
                right: closeBtn.left
                rightMargin: 4
                verticalCenter: parent.verticalCenter
            }
            defaultColor: GUIColor.Clear
            hoveredBkColor: GUIColor.Clear
            pressedBkColor: GUIColor.Clear
            defaultImgPath: "qrc:/Res/info.svg"
            hoveredImgPath: "qrc:/Res/info.svg"
            pressedImgPath: "qrc:/Res/info.svg"
            disabledImgPath: "qrc:/Res/info.svg"
            Component{
                id: infoComponent
                MediaInfoDlg{
                    id: infoDlg
                }
            }
            onClicked: {
                 var obj = infoComponent.createObject(root.parent)
                obj.parent = root.parent
                obj.show()
            }
        }

        GUIImageButton {
            id: closeBtn
            width: 24
            height: 24
            anchors{
                right: parent.right
                rightMargin: 4
                verticalCenter: parent.verticalCenter
            }
            defaultColor: GUIColor.Clear
            hoveredBkColor: GUIColor.Red1
            pressedBkColor: GUIColor.Red1
            defaultImgPath: "qrc:/Res/close.svg"
            hoveredImgPath: "qrc:/Res/close_white.svg"
            pressedImgPath: "qrc:/Res/close.svg"
            disabledImgPath: "qrc:/Res/close.svg"
            onClicked: {
                root.destroy()

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
            bottom: playControlRc.top
        }
        DropArea{
            anchors.fill: parent
            onEntered: {
                drag.accepted = true
            }

            onDropped: {
                dealDropEvent(drop.urls[0].toString())
                console.log( drop.urls[0].toString())
            }
        }
    }
    Rectangle{
        id: playControlRc
        color:  Qt.rgba(45/255,45/255,48/255,1)
        anchors{
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        height: 32
        GUIImageButton {
            id: playControlBtn
            width: 16
            height: 16
            anchors{
                left: parent.left
                leftMargin: 4
                verticalCenter: parent.verticalCenter
            }
            defaultColor: GUIColor.Clear
            hoveredBkColor: GUIColor.Clear
            pressedBkColor: GUIColor.Clear
            defaultImgPath: root.isPlaying? "qrc:/Res/pause_normal.svg":"qrc:/Res/play_normal.svg"
            hoveredImgPath: root.isPlaying? "qrc:/Res/pause_hover.svg":"qrc:/Res/play_hover.svg"
            pressedImgPath: root.isPlaying? "qrc:/Res/pause_hover.svg":"qrc:/Res/play_hover.svg"
            disabledImgPath: root.isPlaying? "qrc:/Res/pause_normal.svg":"qrc:/Res/play_normal.svg"
            onClicked: {
                 var ret = root.isPlaying? viewModel.pause():viewModel.resume()
                if(ret){
                    root.isPlaying = !root.isPlaying
                }
            }
        }
        Slider {
            id: playProgress
            value: 0.0
            property bool needNotifyChange: true
            property var totalTime: 0
            anchors{
                left: playControlBtn.right
                leftMargin: 4
                right: processtxt.left
                rightMargin: 4
                verticalCenter: parent.verticalCenter
            }
            background: Rectangle {
                x: playProgress.leftPadding
                y: playProgress.topPadding + playProgress.availableHeight / 2 - height / 2
                implicitHeight: 4
                height: implicitHeight
                radius: 2
                color: "#bdbebf"

                Rectangle {
                    width: playProgress.visualPosition * parent.width
                    height: parent.height
                    color: "#21be2b"
                    radius: 2
                }
            }

            handle: Rectangle {
                x: playProgress.leftPadding + playProgress.visualPosition * (playProgress.availableWidth - width)
                y: playProgress.topPadding + playProgress.availableHeight / 2 - height / 2
                implicitWidth: 8
                implicitHeight: 8
                radius: 4
                color: playProgress.pressed ? "#f0f0f0" : "#f6f6f6"
                border.color: "#bdbebf"
            }

            function setValue(newValue){
                playProgress.needNotifyChange = false
                playProgress.value = newValue
                playProgress.needNotifyChange = true
            }

            onValueChanged: {
                //console.log(playProgress.value)
                if(!playProgress.needNotifyChange){
                    return
                }
                viewModel.seekTime(playProgress.value * playProgress.totalTime)
            }
       }
       Text{
           id: processtxt
           text:"00:00:00\n00:00:00"
           anchors{
               right: parent.right
               rightMargin: 4
               verticalCenter: parent.verticalCenter
           }
           horizontalAlignment: Text.AlignRight
           verticalAlignment: Text.AlignVCenter
           font: GUIFont.ButtonRegular11
           color: GUIColor.White00
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

    Connections{
        target: viewModel
        onSignalPlayProgressChanged:{
            playProgress.setValue(1.0*currentTime/totalTime)
            playProgress.totalTime = totalTime
            processtxt.text = viewModel.convertUS2String(currentTime) + "\n" + viewModel.convertUS2String(totalTime)
        }
        onSignalPlayStateChanged:{
            root.isPlaying = isPlaying
        }

        onSignalResolutionChanged:{
            root.wh = 1.0*rw/rh
        }
    }
}
