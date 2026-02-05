import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Frame {
    id: card
    padding: 16

    property string title
    property string value
    property string iconName

    ColumnLayout {
        anchors.fill: parent
        spacing: 8

        RowLayout {
            Layout.fillWidth: true
            spacing: 8

            Label {
                text: card.iconName
                font.pixelSize: 16
                color: Material.accent
            }

            Label {
                text: card.title
                font.pixelSize: 14
                color: Material.foreground
                Layout.fillWidth: true
            }
        }

        Label {
            text: card.value
            font.pixelSize: 24
            font.weight: Font.DemiBold
        }
    }
}
