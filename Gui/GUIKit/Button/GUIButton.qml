import QtQuick 2.12
import QtQuick.Controls 2.5
import GUIKit 1.0

Rectangle {
	id:root
    width: 80
    height: 24
    radius: 2
    // 文本属性
    property alias text: btnTxt.text
    property alias font: btnTxt.font
    property alias textAlign: btnTxt.horizontalAlignment
    property color textDefaultColor: "#ffffff"
    property color textHoveredColor: textDefaultColor
    property color textPressedColor: textDefaultColor
    property color textDisabledColor: textDefaultColor

    // 背景属性
    property color defaultColor: GUIColor.Cyan2
    property color defaultMaskColor: Qt.rgba(0, 0, 0, 0)
    property color hoveredBkColor: GUIColor.Cyan2
    property color hoveredMaskColor: Qt.rgba(0, 0, 0, 0.1)
    property color pressedBkColor: GUIColor.Cyan2
    property color pressedMaskColor: Qt.rgba(0, 0, 0, 0.6)
    property color checkedBkColor: GUIColor.Cyan2
    property color checkedMaskColor: Qt.rgba(0, 0, 0, 0)
    property color disablededBkColor: GUIColor.Cyan2
    property color disablededMaskColor:  Qt.rgba(39.0/255, 39.0/255, 42.0/255, 0.9)
    color:root.enabled? (root.down?pressedBkColor:(root.hovered? root.hoveredBkColor: root.defaultColor)):root.disablededBkColor

    // icon
    property string iconSource: ""
    property alias iconWidth: ico.width
    property alias iconHeight: ico.height

    property bool autoRepeat: false // 是否需要重复发送按压消息
    property int autoRepeatDelay: 300 // 按下多久认为是持续按压
    property int autoRepeatInterval: 100 //触发按压信号间隔

    property bool doubleClickEnabled: false
    property int doubleClickInterval: 100

    property bool checkable: false
    property bool checked: false
    property bool hoverEnabled: true
    property bool hovered: false
    property bool down: false

    // tooltip
    property bool showToolTip: false
    property bool enableDelayShowToolTip: false
    property var toolTips: undefined
    property ToolTip toolTip: ToolTip{
        id: toolTipControl
        property bool shouldShow: !root.enableDelayShowToolTip
        visible: (showToolTip && root.toolTips != undefined && shouldShow)?root.hovered:false
        x: 0
        y: 0
        contentItem: Text{
            text: root.toolTips != undefined?  root.toolTips:""
            font.pixelSize: 11
            color: "#d4d4d4"
        }
        background: Rectangle{
            anchors.fill: parent
            color: "#070709"
            radius: 2
        }
    }

    property alias mouseCursorShape: mouseArea.cursorShape

    signal canceled()
    signal entered()
    signal exited()
    signal clicked()
    signal doubleClicked()
    signal pressAndHold()
    signal pressed()
    signal released()
    signal positionChanged(var mouse)

    // 背景
    Rectangle{
        id:maskRc
        anchors.fill: parent
        radius: root.radius
        color:root.enabled? (root.down?pressedMaskColor:(root.hovered? root.hoveredMaskColor: root.defaultMaskColor)):root.disablededMaskColor
    }

    // 文本
    Text {
        id: btnTxt
        text: ""
        font.pixelSize: 12
        elide: Text.ElideRight
        anchors{
            verticalCenter: parent.verticalCenter
            //horizontalCenter: parent.horizontalCenter
            left: ico.visible? ico.right:parent.left
            leftMargin: ico.visible? 4:root.width/2 - btnTxt.width/2
        }

        horizontalAlignment: ico.visible? Text.AlignLeft : Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        color: root.enabled? (root.down? root.textPressedColor: (root.hovered? root.textHoveredColor: root.textDefaultColor)):root.textDisabledColor
    }

    // icon
    Image{
        id: ico
        width: 16
        height: 16
        source: root.iconSource
        visible: root.iconSource != ""
        anchors{
            left: parent.left
            leftMargin: 4
            verticalCenter: parent.verticalCenter
        }
    }

    MouseArea{
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: root.hoverEnabled
        pressAndHoldInterval: root.autoRepeatDelay // MouseArea自有的属性，可以设置按下多久算持续按压

        property bool pressAndHolding: false

        onCanceled: {
            root.canceled()
            onReleasedFunc()
        }

        onEntered: {
            root.hovered = true
            root.entered()
        }

        onExited: {
            root.exited()
            root.hovered = false
        }

        onClicked: {
            if(root.doubleClickEnabled == false){
                if(hovered){
                    if(root.checkable){
                        root.checked = !root.checked
                    }
                    root.clicked()
                }
            }
            else{
                doubleClickTimer.clicked = true
                doubleClickTimer.restart()
            }
        }

        onDoubleClicked: {
            if(root.doubleClickEnabled){
                doubleClickTimer.stop()
                doubleClickTimer.clicked = false
                root.doubleClicked()
            }
        }

        onPositionChanged: (mouse) => {
            if(mouseArea.containsMouse && mouseArea.containsPress){
                root.down = true
            }
            else{
                root.down = false
            }
            root.positionChanged(mouse)
            toolTip.y = mouse.y + 20
        }

        onPressAndHold: {
            if(root.autoRepeat == false){
                pressAndHolding = true
                root.pressAndHold()  // 不重复发就只发一次
            }
            else{
                autoRepeatTimer.restart() // 重复发用定时器，按照设置的触发间隔发送
            }
        }

        onPressed: {
            root.down = true
            root.pressed()
        }

        onReleased: {
            onReleasedFunc()
        }

        function onReleasedFunc(){
            root.down = false
            root.released()
            if(pressAndHolding){
                pressAndHolding = false
                if(hovered){
                    if(root.checkable){
                        root.checked = !root.checked
                    }
                    root.clicked()
                }
            }
            else if(autoRepeatTimer.running){
                autoRepeatTimer.stop()
                if(hovered){
                    if(root.checkable){
                        root.checked = !root.checked
                    }
                    root.clicked()
                }
            }
        }

    }

    onHoveredChanged: {
        if(root.hovered){
            if(root.enableDelayShowToolTip){
                toolTip.shouldShow = false
                showToolTipTimer.restart()
            }
        }
        else if(showToolTipTimer.running){
            showToolTipTimer.stop()
        }
    }

    Timer{
        id: doubleClickTimer
        property bool clicked: false
        interval: root.doubleClickInterval
        repeat: false
        onTriggered: {
            if(doubleClickTimer.clicked){
                doubleClickTimer.clicked = false
                if(root.checkable){
                    root.checked = !root.checked
                }
                root.clicked()
            }
        }
    }

    Timer{
        id: autoRepeatTimer
        interval: root.autoRepeatInterval
        repeat: true
        triggeredOnStart: true
        onTriggered: {
            root.pressAndHold()
        }
    }

    Timer{
        id: showToolTipTimer
        interval: 1000
        repeat: false
        onTriggered: {
            toolTip.shouldShow = true
        }
    }
}
