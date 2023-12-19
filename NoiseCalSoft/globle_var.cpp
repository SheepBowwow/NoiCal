#include "globle_var.h"

int noise_saved_row = 1; // 定义全局变量

DatabaseManager* DBManager = new DatabaseManager(QString("database\\noise_calculation_system.db"));

QMenu *globalMenu = nullptr;

QMap<QString,QVector<QWidget*>> classicRoomMap;

Project project;
