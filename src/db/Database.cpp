#include "Database.h"

#include <QDir>
#include <QObject>
#include <QSqlError>
#include <QSqlQuery>
#include <QStandardPaths>

namespace {
constexpr const char *kConnectionName = "expense_tracker_connection";
}

QSqlDatabase Database::open(QString &errorMessage)
{
    if (QSqlDatabase::contains(kConnectionName)) {
        QSqlDatabase existing = QSqlDatabase::database(kConnectionName);
        if (existing.isOpen()) {
            return existing;
        }
    }

    const QString dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(dataDir);
    if (!dir.exists() && !dir.mkpath(".")) {
        errorMessage = QObject::tr("Unable to create data directory: %1").arg(dataDir);
        return {};
    }

    const QString dbPath = dir.filePath("expenses.sqlite");
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", kConnectionName);
    db.setDatabaseName(dbPath);

    if (!db.open()) {
        errorMessage = db.lastError().text();
        return {};
    }

    if (!migrate(db, errorMessage)) {
        db.close();
        return {};
    }

    return db;
}

bool Database::migrate(QSqlDatabase &db, QString &errorMessage)
{
    QSqlQuery pragmaQuery(db);
    if (!pragmaQuery.exec("PRAGMA user_version")) {
        errorMessage = pragmaQuery.lastError().text();
        return false;
    }

    int version = 0;
    if (pragmaQuery.next()) {
        version = pragmaQuery.value(0).toInt();
    }

    if (version == 0) {
        QSqlQuery createQuery(db);
        if (!createQuery.exec(
                "CREATE TABLE transactions("
                "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                "type INTEGER NOT NULL,"
                "amount_cents INTEGER NOT NULL,"
                "date TEXT NOT NULL,"
                "category TEXT NOT NULL,"
                "note TEXT,"
                "created_at TEXT NOT NULL)")) {
            errorMessage = createQuery.lastError().text();
            return false;
        }

        if (!createQuery.exec("CREATE INDEX idx_transactions_date ON transactions(date)")) {
            errorMessage = createQuery.lastError().text();
            return false;
        }

        if (!createQuery.exec("CREATE INDEX idx_transactions_type ON transactions(type)")) {
            errorMessage = createQuery.lastError().text();
            return false;
        }

        if (!createQuery.exec("CREATE INDEX idx_transactions_category ON transactions(category)")) {
            errorMessage = createQuery.lastError().text();
            return false;
        }

        QSqlQuery versionQuery(db);
        if (!versionQuery.exec("PRAGMA user_version = 1")) {
            errorMessage = versionQuery.lastError().text();
            return false;
        }
    }

    return true;
}
