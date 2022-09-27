import QtQuick 2.12
import QtQuick.Controls 2.0
import GUIKit 1.0

Rectangle {
	id:root
    height: 40
    color: GUIColor.Black1
    property string title:  qsTr("EZPlayer")
    property var titleFont: GUIFont.H5Headline16
    signal signalReqClose()
    Text {
        text:root.title
        font: root.titleFont
        anchors{
           verticalCenter: parent.verticalCenter
           left: parent.left
           leftMargin: 8
        }
        color: GUIColor.Cyan2
    }
    GUIImageButton {
        id: closeBtn
        width: 24
        height: 24
        anchors{
            right: parent.right
            rightMargin: 8
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
            root.signalReqClose()
        }
    }
}
