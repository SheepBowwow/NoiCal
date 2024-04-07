#include "databasemanager.h"
#include<QFile>
#include<QVector>
//有参构造函数 传入文件路径
DatabaseManager::DatabaseManager(const QString& dbName)
{
    db = QSqlDatabase::addDatabase("QSQLITE");      //添加数据库
    db.setDatabaseName(dbName);

    if(!QFile::exists(dbName))
    {
        qDebug() << "数据库文件不存在";
        return;
    }

    if (!db.open())
    {
        qDebug() << "无法打开数据库" << db.lastError().text();
    }
}

//无参构造函数 空实现
DatabaseManager::DatabaseManager()
{
}

DatabaseManager::~DatabaseManager()
{
    db.close();
}

void DatabaseManager::addComponentToDel(const QString &UUID)
{
    ComponentToDelByUUID.append(UUID);
}

QSqlDatabase& DatabaseManager::getDB()
{
    return db;
}
