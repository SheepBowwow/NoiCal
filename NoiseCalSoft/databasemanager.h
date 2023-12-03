#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QCoreApplication>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDebug>
#include <QSqlError>
#include <QDebug>


class DatabaseManager
{
public:
    DatabaseManager(const QString& dbName);

    DatabaseManager();

    ~DatabaseManager();

    QSqlDatabase& getDB();

    bool executeSqlQuery(QVector<QString>& data, const QString &sqlQuery);
    bool queryKnownData(QVector<QString>& data, QString colName, QString tableName);
    bool queryEightNoi(QMap<int,QString>& data, QString tableName, QString modelColName, QString modelName);
private:
    QSqlDatabase db;
};

#endif // DATABASEMANAGER_H
