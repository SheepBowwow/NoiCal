// DatabaseManager.h
#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>

class DatabaseManager {
public:
    static DatabaseManager& instance() {
        static DatabaseManager instance("path_to_your_database.db"); // 使用实际的数据库路径
        return instance;
    }

    QSqlDatabase& getDB() {
        return db;
    }

    // 新增成员函数：从project_basicInfo表中加载projectID
    QSet<QString> loadProjectIDs();

    void addComponentToDel(const QString& UUID);


private:
    QSqlDatabase db;
    // 私有构造函数
    DatabaseManager(const QString& dbName);
    ~DatabaseManager();
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

    QList<QString> componentToDel;
};

#endif // DATABASEMANAGER_H
