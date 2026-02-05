#pragma once

#include "models/TransactionsModel.h"

#include <QDate>
#include <QObject>
#include <QSqlDatabase>
#include <QTimer>

class AppController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QDate currentMonth READ currentMonth NOTIFY currentMonthChanged)
    Q_PROPERTY(QString summaryIncomeFormatted READ summaryIncomeFormatted NOTIFY summaryChanged)
    Q_PROPERTY(QString summaryExpensesFormatted READ summaryExpensesFormatted NOTIFY summaryChanged)
    Q_PROPERTY(QString summaryNetFormatted READ summaryNetFormatted NOTIFY summaryChanged)
    Q_PROPERTY(int summaryIncomeCents READ summaryIncomeCents NOTIFY summaryChanged)
    Q_PROPERTY(int summaryExpensesCents READ summaryExpensesCents NOTIFY summaryChanged)
    Q_PROPERTY(int summaryNetCents READ summaryNetCents NOTIFY summaryChanged)
    Q_PROPERTY(QStringList categories READ categories NOTIFY categoriesChanged)
    Q_PROPERTY(TransactionsModel *transactionsModel READ transactionsModel CONSTANT)
    Q_PROPERTY(bool undoAvailable READ undoAvailable NOTIFY undoAvailableChanged)
    Q_PROPERTY(QString dbErrorMessage READ dbErrorMessage NOTIFY dbErrorMessageChanged)

public:
    explicit AppController(QObject *parent = nullptr);

    QDate currentMonth() const;

    QString summaryIncomeFormatted() const;
    QString summaryExpensesFormatted() const;
    QString summaryNetFormatted() const;
    int summaryIncomeCents() const;
    int summaryExpensesCents() const;
    int summaryNetCents() const;

    QStringList categories() const;

    TransactionsModel *transactionsModel();

    bool undoAvailable() const;
    QString dbErrorMessage() const;

    Q_INVOKABLE void nextMonth();
    Q_INVOKABLE void prevMonth();
    Q_INVOKABLE void setMonth(const QDate &month);

    Q_INVOKABLE bool addTransaction(int type, int amountCents, const QDate &date, const QString &category,
                                    const QString &note);
    Q_INVOKABLE bool updateTransaction(int id, int type, int amountCents, const QDate &date,
                                       const QString &category, const QString &note);
    Q_INVOKABLE bool deleteTransaction(int id);
    Q_INVOKABLE bool undoDelete();

    Q_INVOKABLE void setFilters(int typeFilter, const QString &categoryFilter, const QString &textFilter);

signals:
    void currentMonthChanged();
    void summaryChanged();
    void categoriesChanged();
    void undoAvailableChanged();
    void dbErrorMessageChanged();
    void transactionDeleted();

private:
    struct DeletedTransaction {
        int id = 0;
        int type = 0;
        int amountCents = 0;
        QDate date;
        QString category;
        QString note;
        QString createdAt;
    };

    void refreshSummary();
    void refreshCategories();
    void clearUndo();
    QString formatCents(int cents) const;

    QSqlDatabase m_db;
    TransactionsModel m_model;
    QDate m_currentMonth;
    int m_summaryIncome = 0;
    int m_summaryExpenses = 0;
    QStringList m_categories;
    DeletedTransaction m_lastDeleted;
    bool m_undoAvailable = false;
    QString m_dbErrorMessage;
    QTimer m_undoTimer;
};
