#ifndef GLOBLE_VAR_H
#define GLOBLE_VAR_H
#include <QObject>
#include "databasemanager.h"
#include <QMap>
#include <QVector>

typedef struct _Project
{
    QString prj_name;   //项目名
}Project;


extern int noise_saved_row;     //保存的行数

extern Project project;

extern DatabaseManager* DBManager;

#endif // GLOBLE_VAR_H
