import QtQuick 2.12
import QtQuick.Controls 2.5
import GUIKit 1.0

GUIButton {
	id:root
    width: 24
    height: 24
    property string defaultImgPath: ""
    property string hoveredImgPath: ""
    property string pressedImgPath: ""
    property string disabledImgPath: ""
    property int imgWidth: 18
    property int imgHeight: 18
    Image{
        anchors.centerIn: parent
        width: 18
        height: 18
        source: root.enabled?(root.hovered?root.hoveredImgPath:(root.down? root.pressedImgPath:root.defaultImgPath)):root.disabledImgPath
    }
}
