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

struct MVZ;
struct System;
struct SystemListComp;
struct Room;
struct Outer;
struct ClassicRoom;
struct Duct;

typedef struct SystemListCompDBData {
    QString type;           // 设备类型
    QString number;             // 设备编号
    QString model;          // 设备型号
}SystemListCompDBData;

typedef struct RoomDBData {
    QString number;             // 房间编号
    QString name;           // 房间名称
    QString deck;           // 甲板
    int ductNum;           // 主风管数量
    QString placeType;       // 处所类型
    QString roomType;      // 房间类型
    double limit;              // 噪声限值
    QString isCal;                  //是否计算
    QString referenceNumber;    //引用房间
}RoomDBData;

typedef RoomDBData OuterDBData;

typedef struct DuctDBData
{
    QString number;
    double air_volume;
    bool isNamed;
}DuctDBData;

typedef struct DuctCalTableDBData{
    QString type;
    QString jsonString;
}DuctCalTableDBData;

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

    void delComponentInDatabase(const QString& componentName, const QString &UUID, bool componentDB);
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

    //RoomCalInfo
    //MVZ
    bool addMVZToDatabase(const MVZ*& mvz);
    bool delMVZInDatabase(const MVZ*& mvz);
    bool updateMVZInDatabase(const QString& old_MVZName, const QString& new_MVZName);
    bool getMVZNamesFromDB(QList<QString>& MVZNames);

    //System
    bool addSystemToDatabase(const System*& system);
    bool delSystemInDatabase(const System*& system);
    bool updateSystemInDatabase(const QString& MVZName, const QString& old_systemName, const QString& new_systemName);
    bool getSystemNamesFromDB(const QString& MVZName, QList<QString> &systemNames);

    //SystemListComp
    bool addSystemCompToDatabase(const System*& system, const SystemListComp*& comp);
    bool delSystemCompInDatabase(const System*& system, const SystemListComp *&comp);
    bool updateSystemCompInDatabase(const System*& system, const SystemListComp*& old_comp, const SystemListComp*& new_comp);
    bool getSystemCompsFromDB(const System*& system, QList<SystemListCompDBData> &systemComps);

    //Room
    bool addRoomToDatabase(const Room*& room);
    bool delRoomInDatabase(const Room*& room);
    bool updateRoomInDatabase(const System*& system, const Room*& old_room, const Room*& new_room);
    bool getRoomsFromDB(const System*& system, QList<RoomDBData> &rooms);

    //Outer
    bool addOuterToDatabase(const Outer*& outer);
    bool delOuterInDatabase(const Outer*& outer);
    bool updateOuterInDatabase(const Outer*& old_outer, const Outer*& new_outer);
    bool getOutersFromDB(const MVZ*& mvz, QList<OuterDBData> &outers);

    //Duct
    bool addDuctToDatabase(bool isOuter, const Duct*& duct);
    bool delDuctInDatabase(bool isOuter, const Duct*& duct);
    bool updateDuctInDatabase(bool isOuter, const Duct*& old_duct, const Duct*& new_duct);
    bool getRoomDuctsFromDB(const Room*& room, QList<DuctDBData> &ducts);
    bool getOuterDuctsFromDB(const Outer*& outer, QList<DuctDBData> &ducts);
    bool namedDuct(bool isOuter, const QString& old_number, const Duct*& duct);

    //DuctCalTable
    bool addDuctCalTableToDatabase(bool isOuter, int tableIndex, const QString &type, const QString &tableJsonData,
                         const QString& MVZName, const QString& systemName,
                         const QString& roomOrOuterNumber, const QString& ductNumber);
    bool removeDuctCalTableFromDatabase(bool isOuter, const QString &ductNumber);
    bool getDuctCalTableFromDatabase(bool isOuter, const QString &ductNumber, QList<DuctCalTableDBData> &ductCalTables);


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
