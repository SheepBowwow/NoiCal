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

bool DatabaseManager::executeSqlQuery(QVector<QString>& data,const QString &sqlQuery)
{
    // 执行SELECT查询
    QSqlQuery query;
    query.prepare(sqlQuery);

    if (query.exec())
    {
        while (query.next())
        {
            data.push_back(query.value(0).toString());
        }
    }
    else
    {
        qDebug() << "查询失败:" << query.lastError().text();
        return false;
    }
    return true;
}


bool DatabaseManager::queryKnownData(QVector<QString>& data, QString colName, QString tableName)
{
    // 执行SELECT查询
    QSqlQuery query;
    query.prepare(QString("SELECT " + QString(colName) + " FROM " + QString(tableName))); // 替换为您的表名和列名
    if (query.exec())
    {
        while (query.next())
        {
            data.push_back(query.value(0).toString());
        }
    }
    else
    {
        qDebug() << "查询失败:" << query.lastError().text();
        return false;
    }
    return true;
}

bool DatabaseManager::queryEightNoi(QMap<int,QString>& data, QString tableName, QString modelColName, QString modelName)
{
    QSqlQuery query;
    query.prepare(QString("SELECT `63`, `125`, `250`, `500`, `1000`, `2000`, `4000`, `8000`  FROM " + tableName + " WHERE " + modelColName + " = :modelName")); // 替换为您的表名和列名
    query.bindValue(":modelName", modelName);
    if (query.exec())
    {
        while (query.next())
        {
            // 获取指定列的数据
            QString col63 = query.value("63").toString();
            QString col125 = query.value("125").toString();
            QString col250 = query.value("250").toString();
            QString col500 = query.value("500").toString();
            QString col1000 = query.value("1000").toString();
            QString col2000 = query.value("2000").toString();
            QString col4000 = query.value("4000").toString();
            QString col8000 = query.value("8000").toString();

            // 将数据存入QMap，索引为列名
            data[63] = col63;
            data[125] = col125;
            data[250] = col250;
            data[500] = col500;
            data[1000] = col1000;
            data[2000] = col2000;
            data[4000] = col4000;
            data[8000] = col8000;
        }

        // 查询成功
        return true;
    }
    else
    {
        // 查询失败
        qDebug() << "Query failed: " << query.lastError();
        return false;
    }

}

QSqlDatabase& DatabaseManager::getDB()
{
    return db;
}
