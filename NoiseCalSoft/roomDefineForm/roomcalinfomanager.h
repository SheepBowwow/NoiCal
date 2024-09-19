#ifndef ROOMCALINFOMANAGER_H
#define ROOMCALINFOMANAGER_H

#include <QObject>
#include <QMap>
#include <QList>
#include <QTreeWidgetItem>
#include <QStackedWidget>
#include <QDebug>
#include <QMenu>
#include <QAction>>
#include <QApplication>
#include <tuple>

namespace noise_src_component
{
const QString AIRCONDITION_SINGLE = QStringLiteral("空调器(单风机)");
const QString AIRCONDITION_DOUBLE = QStringLiteral("空调器(双风机)");
const QString FAN = QStringLiteral("风机");
const QString FANCOIL = QStringLiteral("风机盘管");
}

class SystemComp_list_widget;
class Room_define_widget;
class Room_cal_baseWidget;
class Room_cal_total_widget;
class Outer_before_widget;
class Outer_after_widget;

struct SystemListCompDBData;
struct RoomDBData;
typedef RoomDBData OuterDBData;

// 定义主竖区的数据结构
typedef struct MVZ {
    QString name;           // 主竖区名
    QTreeWidgetItem* sysListItem;   //系统清单下的主竖区item
    QTreeWidgetItem* roomItem;      //计算房间下的主竖区item
    QTreeWidgetItem* calItem;       //噪音计算下的主竖区item

    // 定义 operator==
    bool operator==(const MVZ& other) const {
        return std::tie(name, sysListItem, roomItem, calItem) ==
               std::tie(other.name, other.sysListItem, other.roomItem, other.calItem);
    }

    int getItemIndex() const {
        return calItem->parent()->indexOfChild(calItem);
    }
}MVZ;

// 定义系统的数据结构
typedef struct System {
    QString name;           // 系统名
    QTreeWidgetItem* sysListItem;   //系统清单下的系统item
    QTreeWidgetItem* roomItem;      //计算房间下的系统item
    QTreeWidgetItem* calItem;       //噪音计算下的系统item

    SystemComp_list_widget* systemListPage;  //系统清单底下的系统清单页面
    Room_define_widget* roomDefinePage;  //计算房间下的房间定义界面

    // 定义 operator==
    bool operator==(const System& other) const {
        return std::tie(name, sysListItem, roomItem, calItem) ==
               std::tie(other.name, other.sysListItem, other.roomItem, other.calItem);
    }

    int getItemIndex() const {
        return calItem->parent()->indexOfChild(calItem);
    }

    QString getParentMVZName() const {
        return calItem->parent()->text(0);
    }
}System;

// 定义系统清单的数据结构
typedef struct SystemListComp {
    QString type;           // 设备类型
    QString number;             // 设备编号
    QString model;          // 设备型号
    QString uuid;
    int table_index;

    // 定义 operator==
    bool operator==(const SystemListComp& other) const {
        return std::tie(type, number, model, uuid) == std::tie(other.type, other.number, other.model, other.uuid);
    }
}SystemListComp;

// 定义房间的数据结构
typedef struct Room {
    QString number;             // 房间编号
    QString name;           // 房间名称
    QString deck;           // 甲板
    int ductNum;           // 主风管数量
    QString placeType;       // 处所类型
    QString roomType;      // 房间类型
    double limit;              // 噪声限值
    QString isCal;                  //是否计算
    QString referenceRoomNumber;    //引用房间
    QTreeWidgetItem* roomItem;   //房间item
    QTreeWidgetItem* noiSumItem;   //房间噪音item
    Room_cal_total_widget* calTotalPage;    //室外噪音总和界面
    QString systemName;
    QString MVZName;

    // 定义 operator==
    bool operator==(const Room& other) const {
        return std::tie(number, name, deck, ductNum, placeType, roomType, limit) ==
               std::tie(other.number, other.name, other.deck, other.ductNum, other.placeType, other.roomType, other.limit);
    }

    int getItemIndex() const {
        if(!roomItem)
            return -1;
        return roomItem->parent()->indexOfChild(roomItem);
    }
}Room;

// 定义房间的数据结构
typedef struct Outer {
    QString number;             // 房间编号
    QString name;           // 房间名称
    QString deck;           // 甲板
    int ductNum;           // 主风管数量
    QString placeType;       // 处所类型
    QString roomType;      // 房间类型
    double limit;              // 噪声限值
    QString isCal;
    QString referenceOuterNumber;
    QTreeWidgetItem* outerItem;   //房间item
    QTreeWidgetItem* noiSumItem;   //房间噪音item
    QTreeWidgetItem* convergeBeforeItem;   //汇总前叠加item
    QTreeWidgetItem* convergeAfterItem;   //汇总后叠加item
    Room_cal_total_widget* calTotalPage;    //室外噪音总和界面
    Outer_before_widget* convergeBeforePage;    //汇合前叠加界面
    Outer_after_widget* convergeAfterPage;      //汇合后总管界面
    QString MVZName;

    // 定义 operator==
    bool operator==(const Outer& other) const {
        return std::tie(number, name, deck, ductNum, placeType, roomType, limit) ==
               std::tie(other.number, other.name, other.deck, other.ductNum, other.placeType, other.roomType, other.limit);
    }

    QString getParentMVZName() const {
        return outerItem->parent()->parent()->text(0);
    }

    int getItemIndex() const {
        if(!outerItem)
            return -1;
        return outerItem->parent()->indexOfChild(outerItem);
    }
}Outer;

typedef struct ClassicRoom {
    QString name;           // 房间名称
    QTreeWidgetItem* classicRoomItem;   //房间item
    Room_cal_baseWidget* ductCalPage;   //风管计算界面

    // 定义 operator==
    bool operator==(const ClassicRoom& other) const {
        return classicRoomItem == other.classicRoomItem && name == other.name; // 只比较 name，因为 classicRoomItem 是指针
    }
}ClassicRoom;

// 定义风管的数据结构
typedef struct Duct {
    QString number;             // 主风管编号
    int airVolume;         // 风量
    QTreeWidgetItem* ductItem;   //系统清单item
    Room_cal_baseWidget* ductCalPage;   //风管计算界面

    // 定义 operator==
    bool operator==(const Duct& other) const {
        return std::tie(number, airVolume) == std::tie(other.number, other.airVolume);
    }

    QString getParentRoomOrOuterNumber() const{
        return ductItem->parent()->text(0);
    }

    QString getParentMVZName() const {
        return ductItem->parent()->parent()->parent()->text(0);
    }

    QString getParentSystemName() const {
        return ductItem->parent()->parent()->text(0);
    }

    int getItemIndex() const {
        return ductItem->parent()->indexOfChild(ductItem);
    }
}Duct;

class RoomCalInfoManager : public QObject
{
    Q_OBJECT
public:
    // 禁用拷贝构造函数和赋值操作符
    RoomCalInfoManager(const RoomCalInfoManager&) = delete;
    RoomCalInfoManager& operator=(const RoomCalInfoManager&) = delete;

    // 提供一个静态方法来获取类的实例
    static RoomCalInfoManager& getInstance() {
        static RoomCalInfoManager instance; // C++11后，局部静态变量的初始化是线程安全的
        return instance;
    }

    void loadDataFromDB();

    QString getSystemListCompUUID(QString type, QString number, QString model);   //获得系统清单部件的UUID
    QList<QString> getCompUUIDsFromSystemList(const QString& systemName, const QString& compType);

    void createMVZ(QString name);   //创建主竖区
    void deleteMVZ(QString name);
    void createSystem(QString MVZName, QString sysName, bool createOuter = false);    //创建系统
    void deleteSystem(QString name);
    void addCompToSystemList(QString systemName, QString type, QString number, QString model);    //添加部件到系统清单
    void deleteCompInSystemList(QString systemName, QString type, QString number, QString model);
    void updateCompInSystemList(QString systemName, QString old_type, QString old_number, QString old_model,
                                QString new_type, QString new_number, QString new_model);
    void createRoom(QString systemName, QString number, QString name,
                    QString deck, int ductNum, QString placeType, QString roomType, double limit, QString isCal, QString referenceRoomNumber);      //创建房间
    void deleteRoom(QString number);
    void updateRoom(QString systemName, RoomDBData &old_room_data, RoomDBData &new_room_data);
    
    void createOuter(QString MVZName, QString number, QString name,
                     QString deck, int ductNum, QString placeType, QString roomType, double limit,
                     QString isCal, QString referenceOuterNumber);
    void deleteOuter(QString number);
    void updateOuter(QString MVZName, OuterDBData &old_outer_data, OuterDBData &new_outer_data);

    void createDuct(bool isOuter, QString room_or_outer_number, QString ductNumber, int airVolume = -1, bool isNamed = false);

    void deleteDuct(QString number);

    void initWidgetElem(QTreeWidgetItem *item_system_list,
                        QTreeWidgetItem *item_room_define,
                        QTreeWidgetItem *item_room_calculate,
                        QTreeWidgetItem *item_classic_cabin,
                        QTreeWidget *treeWidget,
                        QStackedWidget *stackedWidget);

    QList<QString> getCalRoomNumbers(QString systemName);
    QList<QString> getCalOuterNumbers(QString MVZName);

    RoomDBData getRoomDataByNumber(QString number);
    OuterDBData getOuterDataByNumber(QString number);
private:
    bool loadDataMode{false};
    void loadMVZs();
    void loadSystems();
    void loadSystemListComps();
    void loadRooms();
    void loadOuters();
    void loadDucts();
    void loadDuctCalTable();
    RoomCalInfoManager();

    QList<MVZ*>  MVZs;   //主竖区
    QMap<QString, QList<System*>>   systems;    //系统 <主竖区名, 系统>
    QMap<QString, QList<SystemListComp*>>   systemListComps;    //系统清单 <系统名, 系统清单>
    QMap<QString, QList<Room*>> rooms;      //房间    <系统名, 房间>
    QMap<QString, QList<Outer*>> outers;      //室外    <主竖区名, 室外(类似于房间的定义)>
    QMap<QString, QList<Duct*>> ducts;      //主风管或支管    <房间编号/室外名(主竖区底下的室外), 主风管>
    QList<ClassicRoom*> classicRooms;          //典型房间

    QSet<QString> allNamesSet;

    QTreeWidgetItem* item_system_list;
    QTreeWidgetItem* item_room_define;
    QTreeWidgetItem* item_room_calculate;
    QTreeWidgetItem *item_classic_cabin;
    QTreeWidget* treeWidget;
    QStackedWidget* stackedWidget;

    MVZ* clickedItemInMVZs(QTreeWidgetItem *item);
    System* clickedItemInSystems(QTreeWidgetItem *item);
    ClassicRoom* clickedItemInClassicRooms(QTreeWidgetItem *item);
    Duct* clickedItemInDucts(QTreeWidgetItem *item);
    Room* clickedItemInRooms(QTreeWidgetItem *item);
    Outer* clickedItemInOuters(QTreeWidgetItem *item);

//右键菜单
private:
    // 系统清单的菜单(右键7.系统清单item) 1.添加主竖区
    QMenu*      _menu_systemlist;
    QAction*    _act_add_MVZ;
    // 主竖区的菜单(右键7.系统清单底下的item) 1.添加系统 2.修改主竖区名称 3.删除主竖区
    QMenu*      _menu_MVZ;
    QAction*    _act_add_system;
    QAction*    _act_mod_MVZName;
    QAction*    _act_del_MVZ;
    // 系统的菜单(右键7.系统清单底下的item) 1.修改系统名称 2.删除系统
    QMenu*      _menu_system;
    QAction*    _act_mod_systemName;
    QAction*    _act_del_system;
    // 房间的菜单(右键9.噪音计算底下的主风管编号命名的item) 1.新增主风管
    QMenu*      _menu_room;
    QAction*    _act_add_duct;
    // 室外的菜单(右键9.噪音计算底下的主风管编号命名的item) 1.新增噪音源支管
    QMenu*      _menu_outer;
    QAction*    _act_add_outerDuct;
    // 主风管的菜单(右键9.噪音计算底下的主风管编号命名的item) 1.修改主风管名称 2.删除主风管
    QMenu*      _menu_duct;
    QAction*    _act_del_duct;
    QAction*    _act_mod_ductName;
    //典型住舱的菜单(右键9.噪音计算底下的item) 1.添加典型房间
    QMenu*      _menu_classicRoom_top;
    QAction*    _act_add_classicRoom;
    //典型房间的菜单 1.修改典型房间名称 2.删除典型房间
    QMenu*      _menu_classicRoom_sub;
    QAction*    _act_mod_classicRoomName;
    QAction*    _act_del_classicRoom;
    void initRightButtonMenu();

signals:
    void switchWhitePage();
    void compAddToSystemList(const QString&, const QString&, const QString&, const QString&, const QString&, int&);
    void roomAdd(const QString&, const QString&, const QString&,
                 const QString&, const QString&, const QString&,
                 const QString&, const QString&, const QString&, const QString&);

public slots:
    void slot_treeWidgetItem_pressed(QTreeWidgetItem* item, int index);
};

#endif // ROOMCALINFOMANAGER_H
