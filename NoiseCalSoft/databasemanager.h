// DatabaseManager.h
#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>
#include "project/projectmanager.h"
#include "Component/ComponentStructs.h"
#include <QHash>

extern QHash<QString, QString> typeNameToTableName;



// 定义函数指针类型，用于添加到数据库的操作
using AddToDatabaseFunc = std::function<bool(const ComponentBase&)>;
using UpdateToDatabaseFunc = std::function<bool(const ComponentBase&)>;

class DatabaseManager {
public:
    static DatabaseManager& getInstance() {
        static DatabaseManager instance("database\\noi_cal_database.db"); // 使用实际的数据库路径
        return instance;
    }

    QSqlDatabase& getDB() {
        return db;
    }

    // 新增成员函数：从project_basicInfo表中加载projectID
    QSet<QString> loadProjectIDs();
    ProjectInfo getProjectInfoFromDB(const QString& projectID) const;
    bool getProjectAttachments(const QString& projectID, QList<ProjectAttachment>& attachments);
    bool getProjectDrawings(const QString& projectID, QList<Drawing>& drawings);
    bool getProjectNoiseLimit(const QString& projectID, QList<NoiseLimit>& noiseLimits);

    void registerAddFunctions();
    void registerUpdateFunctions();

    void delComponentInDatabase(const QString& componentName ,const QString &UUID);
    bool delProjectInDatabase(const QString& prjID);
    bool addProjectInfoToDatabase(const ProjectInfo& projectInfo, bool initProject = false);
    bool updateProjectInfoInDatabase(const ProjectInfo &projectInfo);
    bool updateProjectIDInDatabase(const QString& old_prjID, const QString& new_prjID);
    void addDrawingsToDatabase(const QList<Drawing> &drawings, const QString &projectID);
    void addNoiseLimitsToDatabase(const QList<NoiseLimit> &noiseLimits, const QString &projectID);
    bool addAttachmentToDatabase(const ProjectAttachment &attachment, const QString &projectID);
    bool delAttachmentInDatabase(const QString& attachmentName,  const QString &projectID);
    bool isProjectExist(const QString& prjID);
    void loadComponentsFromDatabase();

    QMap<QString, AddToDatabaseFunc> getComponentAddFuncMap() const;

    QMap<QString, AddToDatabaseFunc> getComponentUpdateFuncMap() const;

private:
    QSqlDatabase db;
    // 私有构造函数
    DatabaseManager(const QString& dbName);
    ~DatabaseManager();
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

    QMap<QString, AddToDatabaseFunc> componentAddFuncMap;
    QMap<QString, AddToDatabaseFunc> componentUpdateFuncMap;
};

#endif // DATABASEMANAGER_H
