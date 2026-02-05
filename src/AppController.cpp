#include "AppController.h"

#include "db/Database.h"

#include <QLocale>
#include <QSqlError>
#include <QSqlQuery>
#include <QDateTime>
#include <QDebug>

AppController::AppController(QObject *parent)
    : QObject(parent)
{
    QString errorMessage;
    m_db = Database::open(errorMessage);
    if (!m_db.isOpen()) {
        qWarning() << "Database open failed:" << errorMessage;
        m_dbErrorMessage = errorMessage;
        emit dbErrorMessageChanged();
    }

    m_model.setDatabase(m_db);

    m_currentMonth = QDate::currentDate();
    m_currentMonth = QDate(m_currentMonth.year(), m_currentMonth.month(), 1);

    m_model.setMonth(m_currentMonth);
    refreshSummary();
    refreshCategories();

    m_undoTimer.setInterval(5000);
    m_undoTimer.setSingleShot(true);
    connect(&m_undoTimer, &QTimer::timeout, this, &AppController::clearUndo);
}

QDate AppController::currentMonth() const
{
    return m_currentMonth;
}

QString AppController::summaryIncomeFormatted() const
{
    return formatCents(m_summaryIncome);
}

QString AppController::summaryExpensesFormatted() const
{
    return formatCents(m_summaryExpenses);
}

QString AppController::summaryNetFormatted() const
{
    return formatCents(m_summaryIncome - m_summaryExpenses);
}

int AppController::summaryIncomeCents() const
{
    return m_summaryIncome;
}

int AppController::summaryExpensesCents() const
{
    return m_summaryExpenses;
}

int AppController::summaryNetCents() const
{
    return m_summaryIncome - m_summaryExpenses;
}

QStringList AppController::categories() const
{
    return m_categories;
}

TransactionsModel *AppController::transactionsModel()
{
    return &m_model;
}

bool AppController::undoAvailable() const
{
    return m_undoAvailable;
}

QString AppController::dbErrorMessage() const
{
    return m_dbErrorMessage;
}

void AppController::nextMonth()
{
    setMonth(m_currentMonth.addMonths(1));
}

void AppController::prevMonth()
{
    setMonth(m_currentMonth.addMonths(-1));
}

void AppController::setMonth(const QDate &month)
{
    const QDate normalized(month.year(), month.month(), 1);
    if (normalized == m_currentMonth) {
        return;
    }
    m_currentMonth = normalized;
    emit currentMonthChanged();
    m_model.setMonth(m_currentMonth);
    refreshSummary();
}

bool AppController::addTransaction(int type, int amountCents, const QDate &date,
                                   const QString &category, const QString &note)
{
    if (!m_db.isOpen()) {
        return false;
    }

    QSqlQuery query(m_db);
    query.prepare(
        "INSERT INTO transactions(type, amount_cents, date, category, note, created_at) "
        "VALUES (:type, :amount_cents, :date, :category, :note, :created_at)");
    query.bindValue(":type", type);
    query.bindValue(":amount_cents", amountCents);
    query.bindValue(":date", date.toString("yyyy-MM-dd"));
    query.bindValue(":category", category);
    query.bindValue(":note", note);
    query.bindValue(":created_at", QDateTime::currentDateTimeUtc().toString(Qt::ISODate));

    const bool ok = query.exec();
    if (ok) {
        m_model.reload();
        refreshSummary();
        refreshCategories();
    }

    return ok;
}

bool AppController::updateTransaction(int id, int type, int amountCents, const QDate &date,
                                      const QString &category, const QString &note)
{
    if (!m_db.isOpen()) {
        return false;
    }

    QSqlQuery query(m_db);
    query.prepare(
        "UPDATE transactions SET type = :type, amount_cents = :amount_cents, date = :date, "
        "category = :category, note = :note WHERE id = :id");
    query.bindValue(":type", type);
    query.bindValue(":amount_cents", amountCents);
    query.bindValue(":date", date.toString("yyyy-MM-dd"));
    query.bindValue(":category", category);
    query.bindValue(":note", note);
    query.bindValue(":id", id);

    const bool ok = query.exec();
    if (ok) {
        m_model.reload();
        refreshSummary();
        refreshCategories();
    }

    return ok;
}

bool AppController::deleteTransaction(int id)
{
    if (!m_db.isOpen()) {
        return false;
    }

    QSqlQuery selectQuery(m_db);
    selectQuery.prepare("SELECT id, type, amount_cents, date, category, note, created_at "
                        "FROM transactions WHERE id = :id");
    selectQuery.bindValue(":id", id);
    if (!selectQuery.exec() || !selectQuery.next()) {
        return false;
    }

    m_lastDeleted.id = selectQuery.value(0).toInt();
    m_lastDeleted.type = selectQuery.value(1).toInt();
    m_lastDeleted.amountCents = selectQuery.value(2).toInt();
    m_lastDeleted.date = QDate::fromString(selectQuery.value(3).toString(), "yyyy-MM-dd");
    m_lastDeleted.category = selectQuery.value(4).toString();
    m_lastDeleted.note = selectQuery.value(5).toString();
    m_lastDeleted.createdAt = selectQuery.value(6).toString();

    QSqlQuery deleteQuery(m_db);
    deleteQuery.prepare("DELETE FROM transactions WHERE id = :id");
    deleteQuery.bindValue(":id", id);

    const bool ok = deleteQuery.exec();
    if (ok) {
        m_model.reload();
        refreshSummary();
        refreshCategories();

        m_undoAvailable = true;
        emit undoAvailableChanged();
        emit transactionDeleted();
        m_undoTimer.start();
    }

    return ok;
}

bool AppController::undoDelete()
{
    if (!m_db.isOpen() || !m_undoAvailable) {
        return false;
    }

    QSqlQuery query(m_db);
    query.prepare(
        "INSERT INTO transactions(id, type, amount_cents, date, category, note, created_at) "
        "VALUES (:id, :type, :amount_cents, :date, :category, :note, :created_at)");
    query.bindValue(":id", m_lastDeleted.id);
    query.bindValue(":type", m_lastDeleted.type);
    query.bindValue(":amount_cents", m_lastDeleted.amountCents);
    query.bindValue(":date", m_lastDeleted.date.toString("yyyy-MM-dd"));
    query.bindValue(":category", m_lastDeleted.category);
    query.bindValue(":note", m_lastDeleted.note);
    query.bindValue(":created_at", m_lastDeleted.createdAt);

    const bool ok = query.exec();
    if (ok) {
        clearUndo();
        m_model.reload();
        refreshSummary();
        refreshCategories();
    }

    return ok;
}

void AppController::setFilters(int typeFilter, const QString &categoryFilter, const QString &textFilter)
{
    m_model.setFilters(typeFilter, categoryFilter, textFilter);
}

void AppController::refreshSummary()
{
    if (!m_db.isOpen()) {
        return;
    }

    const QDate firstDay(m_currentMonth.year(), m_currentMonth.month(), 1);
    const QDate lastDay = firstDay.addMonths(1).addDays(-1);

    QSqlQuery query(m_db);
    query.prepare(
        "SELECT type, SUM(amount_cents) "
        "FROM transactions WHERE date BETWEEN :startDate AND :endDate GROUP BY type");
    query.bindValue(":startDate", firstDay.toString("yyyy-MM-dd"));
    query.bindValue(":endDate", lastDay.toString("yyyy-MM-dd"));

    int income = 0;
    int expenses = 0;
    if (query.exec()) {
        while (query.next()) {
            const int type = query.value(0).toInt();
            const int amount = query.value(1).toInt();
            if (type == 1) {
                income = amount;
            } else {
                expenses = amount;
            }
        }
    }

    if (income != m_summaryIncome || expenses != m_summaryExpenses) {
        m_summaryIncome = income;
        m_summaryExpenses = expenses;
        emit summaryChanged();
    }
}

void AppController::refreshCategories()
{
    QStringList categories = {"Food", "Transport", "Housing", "Health", "Other"};

    if (m_db.isOpen()) {
        QSqlQuery query(m_db);
        query.prepare("SELECT DISTINCT category FROM transactions ORDER BY category ASC");
        if (query.exec()) {
            while (query.next()) {
                const QString category = query.value(0).toString();
                if (!categories.contains(category)) {
                    categories.append(category);
                }
            }
        }
    }

    if (categories != m_categories) {
        m_categories = categories;
        emit categoriesChanged();
    }
}

void AppController::clearUndo()
{
    if (!m_undoAvailable) {
        return;
    }
    m_undoAvailable = false;
    emit undoAvailableChanged();
    m_undoTimer.stop();
}

QString AppController::formatCents(int cents) const
{
    const QLocale locale;
    const double amount = static_cast<double>(cents) / 100.0;
    return locale.toString(amount, 'f', 2);
}
