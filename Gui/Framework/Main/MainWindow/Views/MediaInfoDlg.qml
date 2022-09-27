import QtQuick 2.12
//import QtGraphicalEffects 1.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.2
import GUIKit 1.0

Window {
	id:root
    flags: Qt.Dialog | Qt.FramelessWindowHint | Qt.WindowMinMaxButtonsHint | Qt.WindowCloseButtonHint | Qt.WindowSystemMenuHint
    visible: true
    modality: Qt.ApplicationModal
    title: "MediaInfo"
    width: 400
    height: 300

	Rectangle{
		id: bkRc
        anchors.fill: parent
        color: GUIColor.Black0
        border.color: GUIColor.Cyan2
        border.width: 0.5
        Connections{
            target: titleBar
            onSignalReqClose: {
                root.close()
            }
        }
        TitleBar{
            id: titleBar
            title: "MediaInfo"
            titleFont:GUIFont.H5Headline14
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
            id: bodyRc
            color: parent.color
            anchors{
                left: titleBar.left
                top: titleBar.bottom
                right: titleBar.right
                bottom: parent.bottom
                bottomMargin: 1
            }
        }
	}
}
