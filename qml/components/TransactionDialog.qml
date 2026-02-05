import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Dialog {
    id: root
    modal: true
    x: (parent.width - width) / 2
    y: (parent.height - height) / 2
    width: 420
    title: editMode ? qsTr("Edit Transaction") : qsTr("Add Transaction")

    property var categories: []
    property bool editMode: false
    property int editId: 0

    signal saveRequested(var payload)

    function openForAdd() {
        editMode = false
        editId = 0
        typeBox.currentIndex = 0
        amountField.text = ""
        datePicker.selectedDate = new Date()
        categoryBox.currentIndex = 0
        noteField.text = ""
        open()
    }

    function openForEdit(modelData) {
        editMode = true
        editId = modelData.id
        typeBox.currentIndex = modelData.type === 1 ? 1 : 0
        amountField.text = formatAmount(modelData.amountCents)
        datePicker.selectedDate = Qt.dateFromString(modelData.date, "yyyy-MM-dd")
        categoryBox.currentIndex = Math.max(0, categoryBox.model.indexOf(modelData.category))
        noteField.text = modelData.note
        open()
    }

    function formatAmount(cents) {
        return (cents / 100).toFixed(2)
    }

    function parseAmount(text) {
        var normalized = text.trim().replace(/,/g, ".")
        var value = Number(normalized)
        if (isNaN(value) || value <= 0) {
            return 0
        }
        return Math.round(value * 100)
    }

    function commit() {
        var cents = parseAmount(amountField.text)
        if (cents <= 0) {
            amountField.forceActiveFocus()
            return
        }
        var selectedDate = datePicker.selectedDate
        var date = Qt.formatDate(selectedDate, "yyyy-MM-dd")
        root.saveRequested({
            id: editId,
            type: typeBox.currentIndex === 1 ? 1 : 0,
            amountCents: cents,
            date: Qt.dateFromString(date, "yyyy-MM-dd"),
            category: categoryBox.currentText,
            note: noteField.text
        })
        close()
    }

    contentItem: ColumnLayout {
        spacing: 12

        ComboBox {
            id: typeBox
            Layout.fillWidth: true
            model: [qsTr("Expense"), qsTr("Income")]
        }

        TextField {
            id: amountField
            Layout.fillWidth: true
            placeholderText: qsTr("Amount")
            inputMethodHints: Qt.ImhFormattedNumbersOnly
            validator: RegularExpressionValidator { regularExpression: /\d+(?:[\.,]\d{0,2})?/ }
            onEditingFinished: {
                var cents = parseAmount(text)
                if (cents > 0) {
                    text = formatAmount(cents)
                }
            }
        }

        ColumnLayout {
            spacing: 6

            DatePicker {
                id: datePicker
                Layout.fillWidth: true
                selectedDate: new Date()
            }

            Button {
                text: qsTr("Today")
                onClicked: datePicker.selectedDate = new Date()
            }
        }

        ComboBox {
            id: categoryBox
            Layout.fillWidth: true
            model: root.categories
        }

        TextField {
            id: noteField
            Layout.fillWidth: true
            placeholderText: qsTr("Note (optional)")
        }
    }

    footer: DialogButtonBox {
        standardButtons: Dialog.Cancel | Dialog.Ok
        onAccepted: root.commit()
        onRejected: root.close()
    }
}
