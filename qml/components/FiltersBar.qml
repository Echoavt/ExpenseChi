import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Frame {
    id: root
    padding: 12

    property var categories: []

    signal filtersChanged(var payload)

    RowLayout {
        anchors.fill: parent
        spacing: 12

        ButtonGroup { id: typeGroup }

        RowLayout {
            spacing: 6

            Button {
                text: qsTr("All")
                checkable: true
                checked: true
                ButtonGroup.group: typeGroup
            }
            Button {
                text: qsTr("Expense")
                checkable: true
                ButtonGroup.group: typeGroup
            }
            Button {
                text: qsTr("Income")
                checkable: true
                ButtonGroup.group: typeGroup
            }
        }

        ComboBox {
            id: categoryBox
            Layout.preferredWidth: 180
            model: [qsTr("All")].concat(root.categories)
        }

        TextField {
            id: searchField
            Layout.fillWidth: true
            placeholderText: qsTr("Search notes or categories")
            clearButtonEnabled: true
        }
    }

    function currentTypeFilter() {
        if (typeGroup.checkedButton && typeGroup.checkedButton.text === qsTr("Expense")) {
            return 0
        }
        if (typeGroup.checkedButton && typeGroup.checkedButton.text === qsTr("Income")) {
            return 1
        }
        return -1
    }

    function emitFilters() {
        root.filtersChanged({
            typeFilter: currentTypeFilter(),
            categoryFilter: categoryBox.currentText,
            textFilter: searchField.text
        })
    }

    Connections {
        target: typeGroup
        function onCheckedButtonChanged() { root.emitFilters() }
    }

    Connections {
        target: categoryBox
        function onCurrentTextChanged() { root.emitFilters() }
    }

    Connections {
        target: searchField
        function onTextChanged() { root.emitFilters() }
    }

    Component.onCompleted: root.emitFilters()
}
