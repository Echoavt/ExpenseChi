import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    width: 320
    height: 200

    ColumnLayout {
        anchors.centerIn: parent
        spacing: 8

        Label {
            text: qsTr("No transactions yet")
            font.pixelSize: 18
            font.weight: Font.DemiBold
            horizontalAlignment: Text.AlignHCenter
        }

        Label {
            text: qsTr("Add your first income or expense to see it here.")
            color: Material.hintTextColor
            wrapMode: Text.WordWrap
            horizontalAlignment: Text.AlignHCenter
        }
    }
}
