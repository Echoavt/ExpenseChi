import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: root
    width: 360
    height: content.height
    visible: false

    signal actionTriggered()

    property string message: ""
    property string actionText: ""

    Timer {
        id: hideTimer
        interval: 5000
        running: false
        repeat: false
        onTriggered: root.visible = false
    }

    function show(messageText, actionLabel) {
        root.message = messageText
        root.actionText = actionLabel
        root.visible = true
        hideTimer.restart()
    }

    Rectangle {
        id: content
        anchors.fill: parent
        radius: 8
        color: Material.background
        border.color: Material.dividerColor
        height: implicitHeight

        RowLayout {
            anchors.fill: parent
            anchors.margins: 12
            spacing: 12

            Label {
                text: root.message
                Layout.fillWidth: true
            }

            Button {
                text: root.actionText
                visible: root.actionText.length > 0
                onClicked: {
                    root.visible = false
                    root.actionTriggered()
                }
            }
        }
    }
}
