#ifndef GLOBLE_VAR_H
#define GLOBLE_VAR_H
#include <QObject>
#include "databasemanager.h"
#include <QMap>
#include <QVector>
#include <QMenu>

typedef struct _Project
{
    QString prj_name;   //项目名
}Project;


extern int noise_saved_row;     //保存的行数

extern QMenu *globalMenu;       //主风管处的菜单

extern Project project;

extern DatabaseManager* DBManager;  //数据库

extern QMap<QString,QVector<QWidget*>> classicRoomMap;

#endif // GLOBLE_VAR_H
