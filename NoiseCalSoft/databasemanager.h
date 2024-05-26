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
using AddToDatabaseFunc = std::function<bool(const ComponentBase&, bool)>;
using UpdateToDatabaseFunc = std::function<bool(const ComponentBase&, bool)>;

class DatabaseManager {
public:
    static DatabaseManager& getInstance() {
        static DatabaseManager instance;
        return instance;
    }

    QSqlDatabase& getProject_db() {
        return project_db;
    }

    QSqlDatabase& getComponent_db() {
        return component_db;
    }

    // 新增成员函数：从project_basicInfo表中加载projectID
    QSet<QString> loadProjectIDs();
    ProjectInfo getProjectInfoFromDB(const QString& projectID) const;
    bool getProjectAttachments(const QString& projectID, QList<ProjectAttachment>& attachments);
    bool getProjectDrawings(const QString& projectID, QList<Drawing>& drawings);
    bool getProjectNoiseLimit(const QString& projectID, QList<NoiseLimit>& noiseLimits);

    void registerAddFunctions();
    void registerUpdateFunctions();

    void delComponentInDatabase(const QString& componentName , const QString &UUID, bool componentDB);
    bool delProjectInDatabase(const QString& prjID);
    bool addProjectInfoToDatabase(const ProjectInfo& projectInfo, bool initProject = false);
    bool updateProjectInfoInDatabase(const ProjectInfo &projectInfo);
    bool updateProjectIDInDatabase(const QString& old_prjID, const QString& new_prjID);
    void addDrawingsToDatabase(const QList<Drawing> &drawings, const QString &projectID);
    void addNoiseLimitsToDatabase(const QList<NoiseLimit> &noiseLimits, const QString &projectID);
    bool addAttachmentToDatabase(const ProjectAttachment &attachment, const QString &projectID);
    bool delAttachmentInDatabase(const QString& attachmentName,  const QString &projectID);
    bool isProjectExist(const QString& prjID);
    void loadComponentsFromPrjDB();
    void loadComponentsFromComponentDB();

    QMap<QString, AddToDatabaseFunc> getComponentAddFuncMap() const;

    QMap<QString, AddToDatabaseFunc> getComponentUpdateFuncMap() const;

private:
    QSqlDatabase project_db;
    QSqlDatabase component_db;
    // 私有构造函数
    DatabaseManager();  // 移除参数
    ~DatabaseManager();
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

    QMap<QString, AddToDatabaseFunc> componentAddFuncMap;
    QMap<QString, AddToDatabaseFunc> componentUpdateFuncMap;
};

#endif // DATABASEMANAGER_H
