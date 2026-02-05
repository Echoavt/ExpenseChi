#pragma once

#include <QSqlDatabase>
#include <QString>

class Database
{
public:
    static QSqlDatabase open(QString &errorMessage);

private:
    static bool migrate(QSqlDatabase &db, QString &errorMessage);
};
