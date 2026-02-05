#pragma once

#include <QAbstractListModel>
#include <QDate>
#include <QSqlDatabase>

class TransactionsModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Roles {
        IdRole = Qt::UserRole + 1,
        TypeRole,
        AmountFormattedRole,
        AmountCentsRole,
        DateRole,
        CategoryRole,
        NoteRole
    };

    explicit TransactionsModel(QObject *parent = nullptr);

    void setDatabase(const QSqlDatabase &database);
    void setMonth(const QDate &month);

    void setFilters(int typeFilter, const QString &categoryFilter, const QString &textFilter);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    void reload();

    struct Transaction {
        int id = 0;
        int type = 0;
        int amountCents = 0;
        QDate date;
        QString category;
        QString note;
    };

private:
    QString formatCents(int cents) const;

    QList<Transaction> m_items;
    QSqlDatabase m_db;
    QDate m_month;
    int m_typeFilter = -1;
    QString m_categoryFilter;
    QString m_textFilter;
};
