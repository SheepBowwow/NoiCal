// DatabaseManager.cpp
#include "DatabaseManager.h"

DatabaseManager::DatabaseManager(const QString& dbName) {
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dbName);
    if (!db.open()) {
        qDebug() << "无法打开数据库：" << db.lastError().text();
    }
}

DatabaseManager::~DatabaseManager() {
    db.close();
}

<<<<<<< HEAD
QSet<QString> DatabaseManager::loadProjectIDs() {
    QSet<QString> projectIDs;
    if (db.isOpen()) {
        QSqlQuery query(db);
        if (query.exec("SELECT projectID FROM project_basicInfo")) {
            while (query.next()) {
                projectIDs.insert(query.value(0).toString());
            }
        } else {
            qDebug() << "查询失败：" << query.lastError();
        }
    }
    return projectIDs;
=======
void DatabaseManager::addComponentToDel(const QString &UUID)
{
    ComponentToDelByUUID.append(UUID);
}

QSqlDatabase& DatabaseManager::getDB()
{
    return db;
>>>>>>> main
}
