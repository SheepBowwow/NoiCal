#ifndef GLOBLE_VAR_H
#define GLOBLE_VAR_H
#include <QObject>
#include "databasemanager.h"
#include "Component/ComponentStructs.h"
#include "Component/ComponentManager.h".h"
#include <QMap>
#include <QVector>
#include <QMenu>

typedef struct _Project
{
    QString prj_name;   //项目名
}Project;

//噪声要求处的房间
struct Room
{
    QString name;   //房间类型
    QString noise;  //噪音限值
    QString type;   //处所类型
};

extern int noise_saved_row;     //保存的行数

extern QMenu *globalMenu;       //主风管处的菜单

extern Project project;

extern DatabaseManager* DBManager;  //数据库

extern QMap<QString,QVector<QWidget*>> classicRoomMap;

extern QMap<QString,QMap<QString,QVector<QString>>> systemListMap; //第一个QString表示的是系统名，第二个QString是设备类型，三种（空调器、风机、风机盘管）,第三个是他的编号

extern QString reportPath;

extern ComponentManager& componentManager;

extern QVector<Room> rooms;

extern QMap<QString,QString> projectAttachmentMap;     //项目附件，用于附件名称和附件路径的对应

#endif // GLOBLE_VAR_H
