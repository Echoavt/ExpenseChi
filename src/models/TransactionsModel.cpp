#include "TransactionsModel.h"

#include <QLocale>
#include <QSqlError>
#include <QSqlQuery>

TransactionsModel::TransactionsModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

void TransactionsModel::setDatabase(const QSqlDatabase &database)
{
    m_db = database;
}

void TransactionsModel::setMonth(const QDate &month)
{
    if (m_month == month) {
        return;
    }
    m_month = month;
    reload();
}

void TransactionsModel::setFilters(int typeFilter, const QString &categoryFilter, const QString &textFilter)
{
    m_typeFilter = typeFilter;
    m_categoryFilter = categoryFilter;
    m_textFilter = textFilter;
    reload();
}

int TransactionsModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return m_items.count();
}

QVariant TransactionsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_items.count()) {
        return {};
    }

    const Transaction &item = m_items.at(index.row());
    switch (role) {
    case IdRole:
        return item.id;
    case TypeRole:
        return item.type;
    case AmountFormattedRole:
        return formatCents(item.amountCents);
    case AmountCentsRole:
        return item.amountCents;
    case DateRole:
        return item.date.toString("yyyy-MM-dd");
    case CategoryRole:
        return item.category;
    case NoteRole:
        return item.note;
    default:
        return {};
    }
}

QHash<int, QByteArray> TransactionsModel::roleNames() const
{
    return {
        {IdRole, "id"},
        {TypeRole, "type"},
        {AmountFormattedRole, "amountFormatted"},
        {AmountCentsRole, "amountCents"},
        {DateRole, "date"},
        {CategoryRole, "category"},
        {NoteRole, "note"}
    };
}

void TransactionsModel::reload()
{
    if (!m_db.isOpen()) {
        return;
    }

    beginResetModel();
    m_items.clear();

    const QDate firstDay = QDate(m_month.year(), m_month.month(), 1);
    const QDate lastDay = firstDay.addMonths(1).addDays(-1);

    QString queryString =
        "SELECT id, type, amount_cents, date, category, note "
        "FROM transactions WHERE date BETWEEN :startDate AND :endDate";

    if (m_typeFilter != -1) {
        queryString += " AND type = :type";
    }
    if (!m_categoryFilter.isEmpty() && m_categoryFilter != "All") {
        queryString += " AND category = :category";
    }
    if (!m_textFilter.trimmed().isEmpty()) {
        queryString += " AND (note LIKE :search OR category LIKE :search)";
    }
    queryString += " ORDER BY date DESC, id DESC";

    QSqlQuery query(m_db);
    query.prepare(queryString);
    query.bindValue(":startDate", firstDay.toString("yyyy-MM-dd"));
    query.bindValue(":endDate", lastDay.toString("yyyy-MM-dd"));
    if (m_typeFilter != -1) {
        query.bindValue(":type", m_typeFilter);
    }
    if (!m_categoryFilter.isEmpty() && m_categoryFilter != "All") {
        query.bindValue(":category", m_categoryFilter);
    }
    if (!m_textFilter.trimmed().isEmpty()) {
        const QString sanitized = m_textFilter.trimmed().replace('%', "\\%");
        query.bindValue(":search", QString("%%1%").arg(sanitized));
    }

    if (query.exec()) {
        while (query.next()) {
            Transaction item;
            item.id = query.value(0).toInt();
            item.type = query.value(1).toInt();
            item.amountCents = query.value(2).toInt();
            item.date = QDate::fromString(query.value(3).toString(), "yyyy-MM-dd");
            item.category = query.value(4).toString();
            item.note = query.value(5).toString();
            m_items.append(item);
        }
    }

    endResetModel();
}

QString TransactionsModel::formatCents(int cents) const
{
    const QLocale locale;
    const double amount = static_cast<double>(cents) / 100.0;
    return locale.toString(amount, 'f', 2);
}
