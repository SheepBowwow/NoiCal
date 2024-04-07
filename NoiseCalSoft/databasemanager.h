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

    void addComponentToDel(const QString& UUID);
private:
    QSqlDatabase db;

    QList<QString> ComponentToDelByUUID;
};

#endif // DATABASEMANAGER_H
