#include "globle_var.h"

int noise_saved_row = 1; // 定义全局变量

//DatabaseManager* DBManager = new DatabaseManager(QString("\\database\\noise_calculation_system.db"));
DatabaseManager* DBManager;

QMenu *globalMenu = nullptr;

QMap<QString,QVector<QWidget*>> classicRoomMap;

Project project;

QString reportPath = "";

ComponentManager& componentManager = ComponentManager::getInstance();

QMap<QString,QMap<QString,QVector<QString>>> systemListMap;

QMap<QString,QString> projectAttachmentMap;     //项目附件，用于附件名称和附件路径的对应
