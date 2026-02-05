import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Frame {
    id: root
    padding: 12
    property int transactionId: model.id

    signal editRequested()
    signal deleteRequested()

    TapHandler {
        acceptedButtons: Qt.LeftButton
        onTapped: root.editRequested()
    }

    RowLayout {
        anchors.fill: parent
        spacing: 12

        Rectangle {
            width: 8
            height: 48
            radius: 4
            color: model.type === 1 ? Material.accent : Material.primary
        }

        ColumnLayout {
            Layout.fillWidth: true
            spacing: 4

            RowLayout {
                Layout.fillWidth: true
                spacing: 6

                Label {
                    text: model.category
                    font.pixelSize: 16
                    font.weight: Font.DemiBold
                }

                Item { Layout.fillWidth: true }

                Label {
                    text: model.amountFormatted
                    font.pixelSize: 16
                    font.weight: Font.DemiBold
                }
            }

            RowLayout {
                Layout.fillWidth: true
                spacing: 8

                Label {
                    text: model.date
                    color: Material.hintTextColor
                    font.pixelSize: 12
                }

                Label {
                    text: model.note
                    color: Material.hintTextColor
                    font.pixelSize: 12
                    elide: Text.ElideRight
                    Layout.fillWidth: true
                }
            }
        }

        ToolButton {
            text: qsTr("Delete")
            onClicked: root.deleteRequested()
        }
    }
}
