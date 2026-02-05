import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Qt.labs.settings 1.1

import ExpenseTracker 1.0

ApplicationWindow {
    id: window
    width: 1024
    height: 720
    visible: true
    title: qsTr("Expense Tracker")

    Settings {
        id: settings
        property bool darkMode: true
    }

    Material.theme: settings.darkMode ? Material.Dark : Material.Light

    property string monthLabel: Qt.formatDate(appController.currentMonth, "MMMM yyyy")

    header: ToolBar {
        RowLayout {
            anchors.fill: parent
            spacing: 12

            ToolButton {
                text: "‹"
                onClicked: appController.prevMonth()
                accessibleName: qsTr("Previous Month")
            }

            Label {
                Layout.alignment: Qt.AlignVCenter
                font.pixelSize: 20
                font.weight: Font.DemiBold
                text: window.monthLabel
            }

            ToolButton {
                text: "›"
                onClicked: appController.nextMonth()
                accessibleName: qsTr("Next Month")
            }

            Item { Layout.fillWidth: true }

            RowLayout {
                spacing: 6

                Label {
                    text: settings.darkMode ? qsTr("Dark") : qsTr("Light")
                    verticalAlignment: Text.AlignVCenter
                }

                Switch {
                    checked: settings.darkMode
                    onToggled: settings.darkMode = checked
                }
            }
        }
    }

    TransactionDialog {
        id: transactionDialog
        categories: appController.categories
        onSaveRequested: function(payload) {
            if (payload.id === 0) {
                appController.addTransaction(payload.type, payload.amountCents, payload.date,
                                             payload.category, payload.note)
            } else {
                appController.updateTransaction(payload.id, payload.type, payload.amountCents,
                                                payload.date, payload.category, payload.note)
            }
        }
    }

    Snackbar {
        id: snackbar
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 24
        onActionTriggered: appController.undoDelete()
    }

    Dialog {
        id: errorDialog
        modal: true
        title: qsTr("Database Error")
        standardButtons: Dialog.Ok
        visible: appController.dbErrorMessage.length > 0
        contentItem: Label {
            width: 360
            wrapMode: Text.WordWrap
            text: qsTr("We couldn't open the local database. Please check permissions or disk space.\n\n%1")
                .arg(appController.dbErrorMessage)
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 16

        RowLayout {
            Layout.fillWidth: true
            spacing: 12

            DashboardCard {
                Layout.fillWidth: true
                title: qsTr("Income")
                value: appController.summaryIncomeFormatted
                iconName: "arrow_upward"
            }

            DashboardCard {
                Layout.fillWidth: true
                title: qsTr("Expenses")
                value: appController.summaryExpensesFormatted
                iconName: "arrow_downward"
            }

            DashboardCard {
                Layout.fillWidth: true
                title: qsTr("Net")
                value: appController.summaryNetFormatted
                iconName: "account_balance_wallet"
            }
        }

        FiltersBar {
            id: filtersBar
            Layout.fillWidth: true
            categories: appController.categories
            onFiltersChanged: function(payload) {
                appController.setFilters(payload.typeFilter, payload.categoryFilter, payload.textFilter)
            }
        }

        Frame {
            Layout.fillWidth: true
            Layout.fillHeight: true
            padding: 0

            ListView {
                id: transactionsList
                anchors.fill: parent
                clip: true
                model: appController.transactionsModel
                spacing: 8
                delegate: TransactionDelegate {
                    width: ListView.view.width
                    onEditRequested: transactionDialog.openForEdit(model)
                    onDeleteRequested: appController.deleteTransaction(model.id)
                }

                EmptyState {
                    anchors.centerIn: parent
                    visible: transactionsList.count === 0
                }
            }
        }
    }

    RoundButton {
        id: fab
        text: "+"
        font.pixelSize: 28
        width: 56
        height: 56
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.rightMargin: 24
        anchors.bottomMargin: 24
        Material.elevation: 6
        onClicked: transactionDialog.openForAdd()
    }

    Connections {
        target: appController
        function onTransactionDeleted() {
            snackbar.show(qsTr("Deleted"), qsTr("Undo"))
        }
    }
}
