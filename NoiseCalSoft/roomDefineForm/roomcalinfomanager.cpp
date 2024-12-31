#include "roomcalinfomanager.h"
#include "roomDefineForm/systemcomp_list_widget.h"
#include "roomDefineForm/room_define_widget.h"
#include "Component/ComponentStructs.h"
#include "Component/ComponentManager.h"
#include "roomDefineForm/dialog_set_name.h"
#include "roomCal/room_cal_total_widget.h"
#include "roomCal/room_cal_basewidget.h"
#include "roomCal/outer_before_widget.h"
#include "roomCal/outer_after_widget.h"
#include "databasemanager.h"
#include <QJsonObject>


RoomCalInfoManager::RoomCalInfoManager() :
    item_system_list(new QTreeWidgetItem),
    item_room_define(new QTreeWidgetItem),
    item_room_calculate(new QTreeWidgetItem),
    treeWidget(new QTreeWidget),
    stackedWidget(new QStackedWidget)
{
    initRightButtonMenu();
}

void RoomCalInfoManager::loadDataFromDB()
{
    loadDataMode = true;
    loadMVZs();
    loadSystems();
    loadSystemListComps();
    loadRooms();
    loadOuters();
    loadDucts();
    loadDuctCalTable();
    loadDataMode = false;
}

MVZ* RoomCalInfoManager::clickedItemInMVZs(QTreeWidgetItem *item) {
    for (MVZ*& mvz : MVZs) {
        if (mvz->sysListItem == item) {
            return mvz;
        }
    }
    return nullptr;
}

System* RoomCalInfoManager::clickedItemInSystems(QTreeWidgetItem *item) {
    for (auto it = systems.cbegin(); it != systems.cend(); ++it) {
        QList<System*> systemList = it.value();
        for (System* system : systemList) {
            if (system->sysListItem == item) {
                return system;
            }
        }
    }
    return nullptr;
}

ClassicRoom* RoomCalInfoManager::clickedItemInClassicRooms(QTreeWidgetItem *item) {
    for (ClassicRoom*& classicRoom : classicRooms) {
        if (classicRoom->classicRoomItem == item) {
            return classicRoom;
        }
    }
    return nullptr;
}

Duct* RoomCalInfoManager::clickedItemInDucts(QTreeWidgetItem *item) {
    for (auto it = ducts.cbegin(); it != ducts.cend(); ++it) {
        QList<Duct*> ductList = it.value();
        for (Duct*& duct : ductList) {
            if (duct->ductItem == item) {
                return duct;
            }
        }
    }
    return nullptr;
}

Room* RoomCalInfoManager::clickedItemInRooms(QTreeWidgetItem *item) {
    for (auto it = rooms.cbegin(); it != rooms.cend(); ++it) {
        QList<Room*> roomList = it.value();
        for (Room*& room : roomList) {
            if (room->roomItem == item) {
                return room;
            }
        }
    }
    return nullptr;
}

Outer *RoomCalInfoManager::clickedItemInOuters(QTreeWidgetItem *item)
{
    for (auto it = outers.cbegin(); it != outers.cend(); ++it) {
        QList<Outer*> outerList = it.value();
        for (Outer*& outer : outerList) {
            if (outer->outerItem == item) {
                return outer;
            }
        }
    }
    return nullptr;
}

void RoomCalInfoManager::initRightButtonMenu()
{
    //系统清单的菜单(右键7.系统清单item) 1.添加主竖区
    _menu_systemlist = new QMenu();
    _act_add_MVZ = new QAction("添加主竖区");
    _menu_systemlist->addAction(_act_add_MVZ);

    //主竖区的菜单(右键7.系统清单底下的item) 1.添加系统 2.修改主竖区名称 3.删除主竖区
    _menu_MVZ = new QMenu();
    _act_add_system = new QAction("添加系统");
    _act_mod_MVZName = new QAction("修改主竖区名称");
    _act_del_MVZ = new QAction("删除主竖区");
    _menu_MVZ->addAction(_act_add_system);
    _menu_MVZ->addAction(_act_mod_MVZName);
    _menu_MVZ->addAction(_act_del_MVZ);

    //系统的菜单(右键7.系统清单底下的item) 1.修改系统名称 2.删除系统
    _menu_system = new QMenu();
    _act_mod_systemName = new QAction("修改系统名称");
    _act_del_system = new QAction("删除系统");
    _menu_system->addAction(_act_mod_systemName);
    _menu_system->addAction(_act_del_system);

    //房间的菜单(右键9.噪音计算底下的主风管编号命名的item) 1.新增主风管
    _menu_room = new QMenu();
    _act_add_duct = new QAction("新增主风管");
    _menu_room->addAction(_act_add_duct);

    _menu_outer = new QMenu();
    _act_add_outerDuct = new QAction("新增噪音支气管");
    _menu_outer->addAction(_act_add_outerDuct);

    //主风管的菜单(右键9.噪音计算底下的主风管编号命名的item) 1.修改主风管名称 2.删除主风管
    _menu_duct = new QMenu();
    _act_mod_ductName = new QAction("修改主风管名称");
    _act_del_duct = new QAction("删除主风管");
    _menu_duct->addAction(_act_mod_ductName);
    _menu_duct->addAction(_act_del_duct);

    //典型住舱的菜单(右键9.噪音计算底下的item) 1.添加典型房间
    _menu_classicRoom_top = new QMenu();
    _act_add_classicRoom = new QAction("添加典型房间");
    _menu_classicRoom_top->addAction(_act_add_classicRoom);

    //典型房间的菜单 1.修改典型房间名称 2.删除典型房间
    _menu_classicRoom_sub = new QMenu();
    _act_mod_classicRoomName = new QAction("修改典型房间名称");
    _act_del_classicRoom = new QAction("删除典型房间");
    _menu_classicRoom_sub->addAction(_act_mod_classicRoomName);
    _menu_classicRoom_sub->addAction(_act_del_classicRoom);
}

void RoomCalInfoManager::slot_treeWidgetItem_pressed(QTreeWidgetItem *item, int index)
{
    if (qApp->mouseButtons() == Qt::RightButton) // 1、首先判断是否为右键点击
    {
        if(item == item_system_list) //  点击了6.系统清单
        {
            if(_act_add_MVZ == _menu_systemlist->exec(QCursor::pos()))   // 弹出添加主竖区菜单
            {
                Dialog_set_name* dialog = new Dialog_set_name("主竖区名称");
                if(dialog->exec() == QDialog::Accepted)
                {
                    createMVZ(dialog->getName());
                    return;
                }
            }
        }
        // 右击主竖区
        else if(MVZ* mvz = clickedItemInMVZs(item))
        {
            QAction *act = _menu_MVZ->exec(QCursor::pos());
            Dialog_set_name* dialog = nullptr;
            if(act == _act_add_system)   // 添加系统,同时添加对应form
            {
                dialog = new Dialog_set_name("系统编号");
                if(dialog->exec() == QDialog::Accepted)
                {
                    createSystem(item->text(0), dialog->getName());
                }
            }
            else if(act == _act_mod_MVZName)    //修改主竖区名称
            {
                dialog = new Dialog_set_name("主竖区名称");
                if(dialog->exec() == QDialog::Accepted)
                {
                    QString newName = dialog->getName();
                    if(allNamesSet.find(newName) != allNamesSet.end()) {
                        QMessageBox::critical(nullptr, "命名重复", "主竖区 \'" + newName + "\' 已经存在");
                        return;
                    } else {
                        allNamesSet.insert(newName);
                        allNamesSet.remove(mvz->name);
                    }

                    DatabaseManager::getInstance().updateMVZInDatabase(mvz->name, newName);

                    if (systems.contains(mvz->name)) {
                        // 从 QMap 中移除旧的键值对
                        QList<System*> systemsToMove = systems.value(mvz->name);
                        systems.remove(mvz->name);
                        systems.insert(newName, systemsToMove);
                    }

                    mvz->name = dialog->getName();
                    mvz->sysListItem->setText(0, mvz->name);
                    mvz->roomItem->setText(0, mvz->name);
                    mvz->calItem->setText(0, mvz->name);

                    for(auto& system : systems[mvz->name]) {
                        for(auto& room : rooms[system->name]) {
                            room->calTotalPage->setMVZName(mvz->name);
                            for(auto& duct : ducts[room->number]) {
                                duct->ductCalPage->setMVZName(mvz->name);
                            }
                        }
                    }
                }
            }
            else if(act == _act_del_MVZ)        //删除主竖区
            {
                deleteMVZ(mvz->name);
            }
        }
        // 右击系统
        else if(System* system = clickedItemInSystems(item))
        {
            QAction* act = _menu_system->exec(QCursor::pos());
            if(act == _act_mod_systemName) //修改系统编号
            {
                Dialog_set_name *dialog = new Dialog_set_name("系统编号");;
                if(dialog->exec()==QDialog::Accepted)
                {
                    QString newName = dialog->getName();
                    if(allNamesSet.find(newName) != allNamesSet.end()) {
                        QMessageBox::critical(nullptr, "命名重复", "系统 \'" + newName + "\' 已经存在");
                        return;
                    } else {
                        allNamesSet.insert(newName);
                        allNamesSet.remove(system->name);
                    }

                    DatabaseManager::getInstance().updateSystemInDatabase(system->getParentMVZName(), system->name, newName);

                    if (rooms.contains(system->name)) {
                        // 从 QMap 中移除旧的键值对
                        QList<Room*> roomsToMove = rooms.value(system->name);
                        rooms.remove(system->name);
                        rooms.insert(newName, roomsToMove);
                    }

                    if(systemListComps.contains(system->name)) {
                        // 从 QMap 中移除旧的键值对
                        QList<SystemListComp*> compsToMove = systemListComps.value(system->name);
                        systemListComps.remove(system->name);
                        systemListComps.insert(newName, compsToMove);
                    }

                    system->name = dialog->getName();
                    system->sysListItem->setText(0, system->name);
                    system->roomItem->setText(0, system->name);
                    system->calItem->setText(0, system->name);
                }
            }
            else if(act == _act_del_system)   //删除系统
            {
                deleteSystem(system->name);
            }
        }
        //右击房间
        else if(Room* room = clickedItemInRooms(item)) {
            QAction* act = _menu_room->exec(QCursor::pos());
            if(act == _act_add_duct) {
                createDuct(false, room->number, "主风管" + QString::number(room->ductNum + 1));
            }
        }
        //右击室外
        else if(Outer* outer = clickedItemInOuters(item)) {
            QAction* act = _menu_outer->exec(QCursor::pos());
            if(act == _act_add_outerDuct) {
                createDuct(true, outer->number, "噪音支气管" + QString::number(outer->ductNum + 1));
            }
        }
        // 右击主风管
        else if(Duct* duct = clickedItemInDucts(item))
        {
            QAction* act = _menu_duct->exec(QCursor::pos());
            QString dialog_title;
            if(item->parent()->parent()->text(0) == "室外") {
                _act_mod_ductName->setText("修改噪音支气管编号");
                _act_del_duct->setText("删除噪音支气管");
                dialog_title = "噪音支气管编号";
            } else {
                dialog_title = "主风管编号";
            }

            if(act == _act_mod_ductName)
            {
                Dialog_set_name* dialog = new Dialog_set_name(dialog_title);
                if(dialog->exec() == QDialog::Accepted)
                {
                    QString newNumber = dialog->getName();
                    QString originNumber = duct->number;
                    if(allNamesSet.find(newNumber) != allNamesSet.end()) {
                        QMessageBox::critical(nullptr, "命名重复", dialog_title.remove("编号") + " \'" + newNumber + "\' 已经存在");
                        return;
                    } else {
                        allNamesSet.insert(newNumber);
                        allNamesSet.remove(originNumber);
                    }
                    Duct* oldDuct = new Duct(*duct);

                    duct->number = dialog->getName();
                    duct->ductItem->setText(0, duct->number);
                    duct->ductCalPage->setDuctNumber(duct->number);
                    duct->ductCalPage->handle_duct_number_revise(originNumber, newNumber);

                    if(item->parent()->parent()->text(0) == "室外") {
                        DatabaseManager::getInstance().updateDuctInDatabase(true, const_cast<const Duct*&>(oldDuct), const_cast<const Duct*&>(duct));
                    } else {
                        DatabaseManager::getInstance().updateDuctInDatabase(false, const_cast<const Duct*&>(oldDuct), const_cast<const Duct*&>(duct));
                    }
                }
            }
            if(act == _act_del_duct)
            {
                deleteDuct(duct->number);
            }
        }
        // 右击典型舱室
        else if(item == item_classic_cabin)
        {
            if(_act_add_classicRoom == _menu_classicRoom_top->exec(QCursor::pos()))
            {
                Dialog_set_name* dialog = new Dialog_set_name("典型房间名称");
                if(dialog->exec()==QDialog::Accepted)
                {
                    ///TODO 创建典型房间
                    //create_classic_room(box->getname());
                }
            }
        }
        // 右击典型房间
        else if(ClassicRoom* classicRoom = clickedItemInClassicRooms(item))
        {
            //典型房间
            QAction* act = _menu_classicRoom_sub->exec(QCursor::pos());
            if(act == _act_mod_classicRoomName)
            {
                Dialog_set_name* dialog = new Dialog_set_name("典型房间名称");
                if(dialog->exec() == QDialog::Accepted)
                {
                    ///TODO 修改典型房间名称
                }
            }
            else if(act == _act_del_classicRoom)
            {
                    ///删除典型房间
            }
        }
    }
}

QString RoomCalInfoManager::getSystemListCompUUID(QString type, QString number, QString model)
{
    QString compUUID;
    if(type == noise_src_component::FAN)
    {
        QList<QSharedPointer<ComponentBase>> components =
            ComponentManager::getInstance().getComponentsByType(false, component_type_name::FAN);
        for(auto &component: components)
        {
            if(auto fan = dynamic_cast<Fan*>(component.data()))
            {
                if(fan->number == number)
                {
                    compUUID = fan->UUID;
                }
            }
        }
    }
    else if(type == noise_src_component::AIRCONDITION_SINGLE || type == noise_src_component::AIRCONDITION_DOUBLE)
    {
        QList<QSharedPointer<ComponentBase>> components =
            ComponentManager::getInstance().getComponentsByType(false, component_type_name::AIRCONDITION);
        for(auto &component: components)
        {
            if(auto aircondition = dynamic_cast<Aircondition*>(component.data()))
            {
                if(aircondition->send_number == number || (aircondition->send_number + " & " + aircondition->exhaust_number) == number)
                {
                    compUUID = aircondition->UUID;
                }
            }
        }
    }
    else if(type == noise_src_component::FANCOIL)
    {
        QList<QSharedPointer<ComponentBase>> components =
            ComponentManager::getInstance().getComponentsByType(false, component_type_name::FANCOIL);
        for(auto &component: components)
        {
            if(auto fanCoil = dynamic_cast<FanCoil*>(component.data()))
            {
                if(fanCoil->model == model)
                {
                    compUUID = fanCoil->UUID;
                }
            }
        }
    }

    return compUUID;
}

QList<QString> RoomCalInfoManager::getCompUUIDsFromMVZsSystemList(const QString &systemName, const QString &compType)
{
    QString MVZName;
    // 遍历 systems 中的每个 MVZ
    for (auto it = systems.begin(); it != systems.end(); ++it) {
        // 遍历该 MVZ 下的所有系统
        for (auto& system : it.value()) {
            // 如果找到了与 systemName 匹配的系统
            if (system->name == systemName) {
                // 返回该系统所属的 MVZName
                MVZName = it.key();  // it.key() 就是该 MVZ 的名称
                break;
            }
        }
    }

    if(MVZName.isEmpty()) {
        return QList<QString>();
    }

    QList<QString> compUUIDs;
    for(auto& system : systems[MVZName]) {
        QList<QString> sysCompUUIDs = getCompUUIDsFromSystemList(system->name, compType);
        for(auto& compUUID : sysCompUUIDs) {
            compUUIDs.append(compUUID);
        }
    }
    return compUUIDs;
}

QList<QString> RoomCalInfoManager::getCompUUIDsFromSystemList(const QString &systemName, const QString &compType)
{
    QList<QString> compUUIDs;
    for(auto& comp : systemListComps[systemName]) {
        if(comp->type == compType) {
            compUUIDs.append(comp->uuid);
        }
    }

    return compUUIDs;
}

void RoomCalInfoManager::initWidgetElem(QTreeWidgetItem *item_system_list,
                                        QTreeWidgetItem *item_room_define,
                                        QTreeWidgetItem *item_room_calculate,
                                        QTreeWidgetItem *item_classic_cabin,
                                        QTreeWidget *treeWidget,
                                        QStackedWidget *stackedWidget)
{
    this->item_system_list = item_system_list;
    this->item_room_define = item_room_define;
    this->item_room_calculate = item_room_calculate;
    this->item_classic_cabin = item_classic_cabin;
    this->treeWidget = treeWidget;
    this->stackedWidget = stackedWidget;

    // 关联树Item的点击信号，但是在槽中判断是不是右键
    connect(this->treeWidget, &QTreeWidget::itemPressed,
            this, &RoomCalInfoManager::slot_treeWidgetItem_pressed);
}

QList<QString> RoomCalInfoManager::getCalRoomNumbers(QString systemName)
{
    QList<QString> res;
    for(auto& roomList : rooms) {
        for(auto& room : roomList) {
            if(room->isCal == "是" && room->systemName == systemName) {
                res.append(room->number);
            }
        }
    }

    return res;
}

QList<QString> RoomCalInfoManager::getCalOuterNumbers(QString MVZName)
{
    QList<QString> res;
    for(auto& outerList : outers) {
        for(auto& outer : outerList) {
            if(outer->isCal == "是" && outer->MVZName == MVZName) {
                res.append(outer->number);
            }
        }
    }

    return res;
}

RoomDBData RoomCalInfoManager::getRoomDataByNumber(QString number)
{
    RoomDBData data;
    for(auto& roomList : rooms) {
        for(auto& room : roomList) {
            if(room->isCal == "是" && room->number == number) {
                RoomDBData roomData{room->number, room->name, room->deck, room->ductNum,
                                     room->placeType, room->roomType, room->limit, room->isCal, room->referenceRoomNumber};
                data = roomData;
                break;
            }
        }
    }
    return data;
}

OuterDBData RoomCalInfoManager::getOuterDataByNumber(QString number)
{
    OuterDBData data;
    for(auto& outerList : outers) {
        for(auto& outer : outerList) {
            if(outer->isCal == "是" && outer->number == number) {
                OuterDBData outerData{outer->number, outer->name, outer->deck, outer->ductNum,
                                      outer->placeType, outer->roomType, outer->limit, outer->isCal, outer->referenceOuterNumber};
                data = outerData;
                break;
            }
        }
    }
    return data;
}

void RoomCalInfoManager::loadMVZs()
{
    QList<QString> MVZNames;
    DatabaseManager::getInstance().getMVZNamesFromDB(MVZNames);
    for(auto& name : MVZNames) {
        createMVZ(name);
    }
}

void RoomCalInfoManager::loadSystems()
{
    QList<QString> systemNames;
    for(auto& mvz : MVZs){
        QString MVZName = mvz->name;
        DatabaseManager::getInstance().getSystemNamesFromDB(MVZName, systemNames);
        for(auto& name : systemNames) {
            if(name != "室外")
                createSystem(MVZName, name);
        }
    }
}

void RoomCalInfoManager::loadSystemListComps()
{
    for(auto& systemList : systems) {
        for(auto& system : systemList) {
            QList<SystemListCompDBData> compDatas;
            DatabaseManager::getInstance().getSystemCompsFromDB(const_cast<const System*&>(system), compDatas);
            for(auto& data : compDatas) {
                addCompToSystemList(system->name, data.type, data.number, data.model);
            }
        }
    }
}

void RoomCalInfoManager::loadRooms()
{
    for(auto& systemList : systems) {
        for(auto& system : systemList) {
            QList<RoomDBData> roomDatas;
            DatabaseManager::getInstance().getRoomsFromDB(const_cast<const System*&>(system), roomDatas);
            for(auto& data : roomDatas) {
                createRoom(system->name, data.number, data.name, data.deck, data.ductNum, data.placeType, data.roomType, data.limit, data.isCal, data.referenceNumber);
            }
        }
    }
}

void RoomCalInfoManager::loadOuters()
{
    for(auto& mvz : MVZs) {
        QList<OuterDBData> outerDatas;
        DatabaseManager::getInstance().getOutersFromDB(const_cast<const MVZ*&>(mvz), outerDatas);
        for(auto& data : outerDatas) {
            createOuter(mvz->name, data.number, data.name, data.deck, data.ductNum, data.placeType, data.roomType, data.limit, data.isCal, data.referenceNumber);
        }
    }
}

void RoomCalInfoManager::loadDucts()
{
    for(auto& roomList : rooms) {
        for(auto& room : roomList) {
            QList<DuctDBData> ductDatas;
            DatabaseManager::getInstance().getRoomDuctsFromDB(const_cast<const Room*&>(room), ductDatas);
            for(auto& data : ductDatas) {
                createDuct(false, room->number, data.number, data.air_volume, data.isNamed);
            }
        }
    }

    for(auto& outerList : outers) {
        for(auto& outer : outerList) {
            QList<DuctDBData> ductDatas;
            DatabaseManager::getInstance().getOuterDuctsFromDB(const_cast<const Outer*&>(outer), ductDatas);
            for(auto& data : ductDatas) {
                createDuct(true, outer->number, data.number, data.air_volume, data.isNamed);
            }
        }
    }
}

void RoomCalInfoManager::loadDuctCalTable()
{
    for(auto& ductList : ducts) {
        for(auto& duct : ductList) {
            QList<DuctCalTableDBData> ductCalTables;
            bool isOuter = duct->ductCalPage->getIsOuter();
            DatabaseManager::getInstance().getDuctCalTableFromDatabase(isOuter, duct->number, ductCalTables);
            for(auto& data : ductCalTables) {
                QJsonObject jsonObj = QJsonDocument::fromJson(data.jsonString.toUtf8()).object();
                duct->ductCalPage->addTable(stringToRoomCalTableType(data.type), jsonObj);
            }
            duct->ductCalPage->loadDataToCalTotalTable();
        }
    }
}

///TODO 去widget.cpp 连接 switchWhitePage
void RoomCalInfoManager::createMVZ(QString name)
{
    if(allNamesSet.find(name) != allNamesSet.end()) {
        QMessageBox::critical(nullptr, "命名重复", "主竖区 \'" + name + "\' 已经存在");
        return;
    } else {
        allNamesSet.insert(name);
    }

    QTreeWidgetItem *sysListItem = new QTreeWidgetItem(item_system_list,QStringList(name));
    QTreeWidgetItem *roomItem = new QTreeWidgetItem(item_room_define,QStringList(name));
    QTreeWidgetItem *calItem = new QTreeWidgetItem(QStringList(name));

    if(!sysListItem || !roomItem || !calItem) {
        allNamesSet.remove(name);
        qDebug() << "treeItem 未正确初始化";
        return;
    }

    //  保证典型房间一直在最后一行
    int n = item_room_calculate->childCount();
    int insertIndex = qMax(0, n - 1);
    item_room_calculate->insertChild(insertIndex, calItem);

    MVZ* mvz = new MVZ{name, sysListItem, roomItem, calItem};
    MVZs.append(mvz);

    if(!loadDataMode)
        DatabaseManager::getInstance().addMVZToDatabase(const_cast<const MVZ*&>(mvz));

    connect(treeWidget, &QTreeWidget::itemClicked,this,[=](QTreeWidgetItem *item, int n){
        if(item == sysListItem || item == roomItem || item == calItem)
            emit switchWhitePage();
    });

    //创建室外
    createSystem(name, "室外", true);
}

void RoomCalInfoManager::deleteMVZ(QString name)
{   
    MVZ* mvz = nullptr;
    for(auto& findMVZ : MVZs) {
        if(findMVZ->name == name) {
            mvz = findMVZ;
            break;
        }
    }

    if(!mvz) {
        qDebug() << "mvz is not found";
        return;
    }

    allNamesSet.remove(name);

    for(auto& system : systems[name]) {
        for(auto& room : rooms[system->name]) {
            //删除主竖区下的所有主风管
            for(auto& duct : ducts[room->name]) {
                stackedWidget->removeWidget(duct->ductCalPage);
                delete duct->ductCalPage;
                delete duct;
            }
            ducts[room->name].clear();
            if(room->ductNum >= 1)
            {
                stackedWidget->removeWidget(room->calTotalPage);
                delete room->calTotalPage;
            }
            delete room;
        }
        rooms[system->name].clear();
        stackedWidget->removeWidget(system->roomDefinePage);
        delete system->roomDefinePage;
        stackedWidget->removeWidget(system->systemListPage);
        delete system->systemListPage;
        delete system;
    }
    //删除主竖区下的系统
    systems[name].clear();
    for(auto& outer : outers[name]) {
        if(outer->ductNum >= 1)
        {
            stackedWidget->removeWidget(outer->calTotalPage);
            delete outer->calTotalPage;
        }
        if(outer->ductNum >= 2)
        {
            stackedWidget->removeWidget(outer->convergeBeforePage);
            stackedWidget->removeWidget(outer->convergeAfterPage);
            delete outer->convergeBeforePage;
            delete outer->convergeAfterPage;
        }
        delete outer;
    }
    //删除室外
    outers[name].clear();
    //删除室外支管
    ducts[name].clear();
    MVZs.removeOne(mvz);

    const MVZ* constMVZ = mvz;
    DatabaseManager::getInstance().delMVZInDatabase(constMVZ);

    delete mvz->calItem;
    delete mvz->roomItem;
    delete mvz->sysListItem;
    delete mvz;
}

void RoomCalInfoManager::createSystem(QString MVZName, QString sysName, bool createOuter)
{
    if(sysName != "室外") {
        if(allNamesSet.find(sysName) != allNamesSet.end()) {
            QMessageBox::critical(nullptr, "命名重复", "系统 \'" + sysName + "\' 已经存在");
            return;
        } else {
            allNamesSet.insert(sysName);
        }
    }

    SystemComp_list_widget *systemListPage;
    Room_define_widget *roomDefinePage;

    QTreeWidgetItem *MVZSysListItem = nullptr;
    QTreeWidgetItem* MVZRoomItem = nullptr;
    QTreeWidgetItem* MVZCalItem = nullptr;

    for(auto& mvz : MVZs) {
        if(mvz->name == MVZName) {
            MVZSysListItem = mvz->sysListItem;
            MVZRoomItem = mvz->roomItem;
            MVZCalItem = mvz->calItem;
            break;
        }
    }

    if(!MVZSysListItem || !MVZRoomItem || !MVZCalItem) {
        allNamesSet.remove(sysName);
        qDebug() << "主竖区不存在";
        return;
    }

    QTreeWidgetItem *sysListItem = new QTreeWidgetItem(QStringList(sysName));
    QTreeWidgetItem *roomItem = new QTreeWidgetItem(QStringList(sysName));
    QTreeWidgetItem *calItem = new QTreeWidgetItem(QStringList(sysName));

    int childCount = MVZCalItem->childCount() - 1;
    int outerIndex = qMax(0, childCount);

    //不是室外才添加系统清单
    if(!createOuter) {
        MVZSysListItem->insertChild(outerIndex, sysListItem);
    }
    else {
        delete sysListItem;
        sysListItem = nullptr;
    }
    MVZRoomItem->insertChild(outerIndex, roomItem);
    MVZCalItem->insertChild(outerIndex, calItem);

    systemListPage = new SystemComp_list_widget(sysName);
    stackedWidget->addWidget(systemListPage);
    connect(treeWidget, &QTreeWidget::itemClicked,this,[=](QTreeWidgetItem *item, int n){
        if(item == sysListItem)     //     点击系统会显示系统清单界面
            stackedWidget->setCurrentWidget(systemListPage);
    });

    //创建室外就采用室外的界面
    if(createOuter) {
        roomDefinePage = new Room_define_widget(MVZName + "室外");
        roomDefinePage->switchOuter();
    } else {
        roomDefinePage = new Room_define_widget(sysName);
    }

    roomDefinePage->setSystemItem(roomItem);   // 房间位于的系统item
    stackedWidget->addWidget(roomDefinePage);

    connect(treeWidget, &QTreeWidget::itemClicked, this, [=](QTreeWidgetItem *item,int n) {
        if(item == roomItem)     //     点击系统会显示添加的页面
            stackedWidget->setCurrentWidget(roomDefinePage);
    });

    System* system = new System{sysName, sysListItem, roomItem, calItem, systemListPage, roomDefinePage};
    systems[MVZName].append(system);  //将systm添加到系统容器的对应主竖区下

    if(!loadDataMode)
        DatabaseManager::getInstance().addSystemToDatabase(const_cast<const System*&>(system));
}

void RoomCalInfoManager::deleteSystem(QString name)
{
    System* system = nullptr;
    bool found = false;
    for(auto& systemList : systems) {
        for(auto& findSystem : systemList) {
            if(findSystem->name == name) {
                system = findSystem;
                found = true;
                break;
            }
        }
        if(found) {
            break;
        }
    }

    if(!system) {
        qDebug() << "system is not found";
        return;
    }

    if(name != "室外")
        allNamesSet.remove(name);

    for(auto& room : rooms[name]) {
        //删除主竖区下的所有主风管
        for(auto& duct : ducts[room->name]) {
            stackedWidget->removeWidget(duct->ductCalPage);
            delete duct->ductCalPage;
            delete duct;
        }
        ducts[room->name].clear();
        if(room->ductNum >= 1)
        {
            stackedWidget->removeWidget(room->calTotalPage);
            delete room->calTotalPage;
        }
        delete room;
    }

    //删除系统下的所有房间
    rooms[name].clear();

    //删除容器中的系统
    QString MVZName = system->calItem->parent()->text(0);
    systems[MVZName].removeOne(system);

    DatabaseManager::getInstance().delSystemInDatabase(const_cast<const System*&>(system));

    delete system->calItem;
    delete system->roomItem;
    delete system->sysListItem;

    stackedWidget->removeWidget(system->systemListPage);
    delete system->systemListPage;
    stackedWidget->removeWidget(system->roomDefinePage);
    delete system->roomDefinePage;

    delete system;
}

void RoomCalInfoManager::addCompToSystemList(QString systemName, QString type, QString number, QString model)
{
    QString compUUID = getSystemListCompUUID(type, number, model);

    for(auto& comp : systemListComps[systemName]) {
        if(comp->uuid == compUUID) {
            QMessageBox::critical(nullptr, "重复添加", "部件已经被添加到该系统清单\n请勿重复添加");
            return;
        }
    }

    int table_index = -1;
    emit compAddToSystemList(systemName, type, number, model, compUUID, table_index);
    SystemListComp* comp = new SystemListComp{type, number, model, compUUID, table_index};
    systemListComps[systemName].append(comp);

    const System* system;
    bool found = false;
    for(auto& systemList : systems) {
        for(auto& findSystem : systemList) {
            if(findSystem->name == systemName) {
                found = true;
                system = findSystem;
                break;
            }
        }
        if(found) {
            break;
        }
    }

    if(!found) {
        qDebug() << "system is not found";
        return;
    }

    if(!loadDataMode) {
        DatabaseManager::getInstance().addSystemCompToDatabase(system, const_cast<const SystemListComp*&>(comp));
        emit
    }
}

void RoomCalInfoManager::deleteCompInSystemList(QString systemName, QString type, QString number, QString model)
{
    QString compUUID = getSystemListCompUUID(type, number, model);

    const System* system;
    bool found = false;
    for(auto& systemList : systems) {
        for(auto& findSystem : systemList) {
            if(findSystem->name == systemName) {
                found = true;
                system = findSystem;
                break;
            }
        }
        if(found) {
            break;
        }
    }

    if(!found) {
        qDebug() << "system is not found";
        return;
    }

    SystemListComp* comp = nullptr;
    for(auto& findComp : systemListComps[systemName]) {
        if(findComp->type == type && findComp->number == number && findComp->model == model && findComp->uuid == compUUID) {
            comp = findComp;
            break;
        }
    }

    if(!comp) {
        qDebug() << "comp is not found";
        return;
    }

    systemListComps[systemName].removeOne(comp);

    DatabaseManager::getInstance().delSystemCompInDatabase(system, const_cast<const SystemListComp*&>(comp));
}

void RoomCalInfoManager::updateCompInSystemList(QString systemName, QString old_type, QString old_number, QString old_model,
                                                QString new_type, QString new_number, QString new_model)
{
    QString old_compUUID = getSystemListCompUUID(old_type, old_number, old_model);
    QString new_compUUID = getSystemListCompUUID(new_type, new_number, new_model);
    const System* system;
    bool found = false;
    for(auto& systemList : systems) {
        for(auto& findSystem : systemList) {
            if(findSystem->name == systemName) {
                found = true;
                system = findSystem;
                break;
            }
        }
        if(found) {
            break;
        }
    }

    if(!found) {
        qDebug() << "system is not found";
        return;
    }

    SystemListComp* old_comp = nullptr;
    for(auto& findComp : systemListComps[systemName]) {
        if(findComp->type == old_type && findComp->number == old_number && findComp->model == old_model && findComp->uuid == old_compUUID) {
            old_comp = findComp;
            break;
        }
    }

    if(!old_comp) {
        qDebug() << "comp is not found";
        return;
    }
    SystemListComp* new_comp = new SystemListComp{new_type, new_number, new_model, new_compUUID, old_comp->table_index};
    DatabaseManager::getInstance().updateSystemCompInDatabase(system, const_cast<const SystemListComp*&>(old_comp), const_cast<const SystemListComp*&>(new_comp));
    int index = systemListComps[systemName].indexOf(old_comp);
    systemListComps[systemName].removeOne(old_comp);
    delete old_comp;
    systemListComps[systemName].insert(index, new_comp);
}

void RoomCalInfoManager::createRoom(QString systemName, QString roomNumber, QString roomName,
                                    QString deck, int ductNum, QString placeType, QString roomType,
                                    double limit, QString isCal, QString referenceRoomNumber)
{
    if(allNamesSet.find(roomNumber) != allNamesSet.end()) {
        QMessageBox::critical(nullptr, "命名重复", "房间 \'" + roomNumber + "\' 已经存在");
        return;
    } else {
        allNamesSet.insert(roomNumber);
    }

    QTreeWidgetItem* roomItem = nullptr;
    QTreeWidgetItem* systemItem = nullptr;
    System* system = nullptr;
    for (auto it = systems.begin(); it != systems.end(); ++it) {
        QList<System*>& systemList = it.value();
        for (System* findSystem : systemList) {
            if (findSystem->name == systemName) {
                systemItem = findSystem->calItem;
                system = findSystem;
                break;
            }
        }
    }

    if(!systemItem || !system)
    {
        allNamesSet.remove(roomNumber);
        qDebug() << "systemItem not exist";
        return;
    }

    if(isCal == "是")
        roomItem = new QTreeWidgetItem(systemItem, QStringList(roomNumber));

    //创建房间时风管初始化为0, 房间噪音为nullptr
    QTreeWidgetItem* noiSumItem = nullptr;
    Room_cal_total_widget* calTotalPage = nullptr;
    int initDuctNum = 0;    //先初始化为0

    Room* room = new Room{roomNumber, roomName, deck, initDuctNum, placeType, roomType,
                          limit, isCal, referenceRoomNumber, roomItem, noiSumItem, calTotalPage};
    room->systemName = systemItem->text(0);
    room->MVZName = systemItem->parent()->text(0);
    if(room->isCal == "否") {
        room->ductNum = ductNum;
    }
    rooms[systemName].append(room);

    if(!loadDataMode) {
        DatabaseManager::getInstance().addRoomToDatabase(const_cast<const Room*&>(room));
        for(int i = 0; i < ductNum; i++) {
            if(isCal == "是")
                createDuct(false, roomNumber, "主风管" + QString::number(room->ductNum + 1));
        }
    }

    emit roomAdd(systemName, roomNumber, roomName, deck, QString::number(room->ductNum), placeType,
                 roomType, QString::number(limit), isCal, referenceRoomNumber);
}

void RoomCalInfoManager::deleteRoom(QString number)
{
    Room* room = nullptr;
    bool found = false;
    for(auto& roomList : rooms) {
        for(auto& findRoom : roomList) {
            if(findRoom->number == number) {
                room = findRoom;
                found = true;
                break;
            }
        }
        if(found) {
            break;
        }
    }

    if(!room) {
        qDebug() << "room is not found";
        return;
    }

    QString systemName = room->systemName;
    System* system = nullptr;
    found = false;
    for(auto& systemList : systems) {
        for(auto findSystem : systemList) {
            if(findSystem->name == systemName) {
                system = findSystem;
                found = true;
                break;
            }
        }
        if(found) {
            break;
        }
    }

    if(!system) {
        qDebug() << "system is not found";
        return;
    }

    allNamesSet.remove(number);
    ducts[room->number].clear();
    rooms[systemName].removeOne(room);

    DatabaseManager::getInstance().delRoomInDatabase(const_cast<const Room*&>(room));

    if(room->roomItem)
        delete room->roomItem;
    if(room->ductNum >= 1) {
        stackedWidget->removeWidget(room->calTotalPage);
        delete room->calTotalPage;
    }
    delete room;
}

void RoomCalInfoManager::updateRoom(QString systemName, RoomDBData& old_room_data, RoomDBData& new_room_data)
{
    System* system = nullptr;
    bool found = false;
    for(auto& systemList : systems) {
        for(auto findSystem : systemList) {
            if(findSystem->name == systemName) {
                system = findSystem;
                found = true;
                break;
            }
        }
        if(found) {
            break;
        }
    }

    if(!system) {
        qDebug() << "system is not found";
        return;
    }

    Room* oldRoom = nullptr;
    for(auto& room : rooms[systemName]) {
        if(room->number == old_room_data.number &&
            room->name == old_room_data.name) {
            oldRoom = room;
            break;
        }
    }
    if(!oldRoom) {
        qDebug() << "room is not found";
        return;
    }

    Room* newRoom = new Room{new_room_data.number, new_room_data.name, new_room_data.deck, new_room_data.ductNum,
                             new_room_data.placeType, new_room_data.roomType, new_room_data.limit,
                             new_room_data.isCal, new_room_data.referenceNumber,
                             oldRoom->roomItem, oldRoom->noiSumItem, oldRoom->calTotalPage};

    DatabaseManager::getInstance().updateRoomInDatabase(const_cast<const System*&>(system)
                                                        , const_cast<const Room*&>(oldRoom)
                                                        ,const_cast<const Room*&>(newRoom));

    for(auto& room : rooms[systemName]) {
        if(room->number == old_room_data.number &&
            room->name == old_room_data.name) {
            room->number = new_room_data.number;
            room->name = new_room_data.name;
            room->deck = new_room_data.deck;
            room->ductNum = new_room_data.ductNum;
            room->placeType = new_room_data.placeType;
            room->roomType = new_room_data.roomType;
            room->limit = new_room_data.limit;

            if(room->isCal == "是") {
                room->roomItem->setText(0, new_room_data.number);
                room->calTotalPage->setInfo(room->roomItem->parent()->parent()->text(0), new_room_data.deck,
                                            new_room_data.number, QString::number(new_room_data.limit),
                                            QString::number(new_room_data.ductNum));
            }

            break;
        }
    }

    if(ducts.contains(old_room_data.number)) {
        QList<Duct*> ductsToMove = ducts.value(old_room_data.number);
        for(auto& duct : ductsToMove) {
            duct->ductCalPage->setInfo(duct->ductItem->parent()->parent()->parent()->text(0),
                                       new_room_data.deck,
                                       new_room_data.number);
        }
        ducts.remove(old_room_data.number);
        ducts.insert(new_room_data.number, ductsToMove);
    }
}

void RoomCalInfoManager::createOuter(QString MVZName, QString outerNumber, QString outerName,
                                     QString deck, int ductNum, QString placeType,
                                     QString roomType, double limit, QString isCal, QString referenceOuterNumber)
{
    if(allNamesSet.find(outerNumber) != allNamesSet.end()) {
        QMessageBox::critical(nullptr, "命名重复", "主竖区 \'" + outerNumber + "\' 已经存在");
        return;
    } else {
        allNamesSet.insert(outerNumber);
    }

    QTreeWidgetItem* outerCalItem = nullptr;
    QTreeWidgetItem* outerItem = nullptr;
    
    const QList<System*>& systemList = systems[MVZName];

    for (auto& system : systemList) {
        if (system->name == "室外") {
            outerCalItem = system->calItem;
            break;
        }
    }

    if(!outerCalItem)
    {
        allNamesSet.remove(outerNumber);
        qDebug() << "outerCalItem not exist";
        return;
    }
    if(isCal == "是")
        outerItem = new QTreeWidgetItem(outerCalItem, QStringList(outerNumber));

    QTreeWidgetItem* noiSumItem = nullptr;
    Room_cal_total_widget* calTotalPage = nullptr;
    QTreeWidgetItem* convergeBeforeItem = nullptr;
    Outer_before_widget* convergeBeforePage = nullptr;
    QTreeWidgetItem* convergeAfterItem = nullptr;
    Outer_after_widget* convergeAfterPage = nullptr;
    int initDuctNum = 0;
    
    Outer* outer = new Outer{outerNumber, outerName, deck, initDuctNum, placeType, roomType, limit, isCal,
                             referenceOuterNumber, outerItem, noiSumItem, convergeBeforeItem, convergeAfterItem,
                             calTotalPage, convergeBeforePage, convergeAfterPage};
    outer->MVZName = outerCalItem->parent()->text(0);
    if(isCal == "否") {
        outer->ductNum = ductNum;
    }
    outers[MVZName].append(outer);

    if(!loadDataMode) {
        DatabaseManager::getInstance().addOuterToDatabase(const_cast<const Outer*&>(outer));
        for(int i = 0; i < ductNum; i++) {
            if(isCal == "是")
                createDuct(true, outerNumber, "噪音源支管" + QString::number(outer->ductNum + 1));
        }
    }

    emit roomAdd(MVZName + "室外", outerNumber, outerName, deck, QString::number(outer->ductNum),
                 placeType, roomType, QString::number(limit), isCal, referenceOuterNumber);
}

void RoomCalInfoManager::deleteOuter(QString number)
{
    Outer* outer = nullptr;
    bool found = false;
    for(auto& outerList : outers) {
        for(auto& findOuter : outerList) {
            if(findOuter->number == number) {
                outer = findOuter;
                found = true;
                break;
            }
        }
        if(found) {
            break;
        }
    }

    if(!outer) {
        qDebug() << "outer is not found";
        return;
    }

    allNamesSet.remove(number);

    ducts[outer->number].clear();
    QString MVZName = outer->outerItem->parent()->parent()->text(0);
    outers[MVZName].removeOne(outer);

    DatabaseManager::getInstance().delOuterInDatabase(const_cast<const Outer*&>(outer));

    if(outer->outerItem)
        delete outer->outerItem;
    if(outer->ductNum >= 1) {
        stackedWidget->removeWidget(outer->calTotalPage);
        delete outer->calTotalPage;
    }
    if(outer->ductNum >= 2) {
        stackedWidget->removeWidget(outer->convergeBeforePage);
        stackedWidget->removeWidget(outer->convergeAfterPage);
        delete outer->convergeBeforePage;
        delete outer->convergeAfterPage;
    }
    delete outer;
}

void RoomCalInfoManager::updateOuter(QString MVZName, OuterDBData &old_outer_data, OuterDBData &new_outer_data)
{
    Outer* oldOuter = nullptr;
    for(auto& outer : outers[MVZName]) {
        if(outer->number == old_outer_data.number &&
            outer->name == old_outer_data.name) {
            oldOuter = outer;
            break;
        }
    }
    if(!oldOuter) {
        qDebug() << "outer is not found";
        return;
    }

    Outer* newOuter = new Outer{new_outer_data.number, new_outer_data.name, new_outer_data.deck, new_outer_data.ductNum,
                             new_outer_data.placeType, new_outer_data.roomType, new_outer_data.limit, new_outer_data.isCal,
                                new_outer_data.referenceNumber, oldOuter->outerItem, oldOuter->noiSumItem, oldOuter->convergeBeforeItem
                               , oldOuter->convergeAfterItem, oldOuter->calTotalPage
                               , oldOuter->convergeBeforePage, oldOuter->convergeAfterPage};

    DatabaseManager::getInstance().updateOuterInDatabase(const_cast<const Outer*&>(oldOuter)
                                                        , const_cast<const Outer*&>(newOuter));

    for(auto& outer : outers[MVZName]) {
        if(outer->number == old_outer_data.number &&
            outer->name == old_outer_data.name) {
            outer->number = new_outer_data.number;
            outer->name = new_outer_data.name;
            outer->deck = new_outer_data.deck;
            outer->ductNum = new_outer_data.ductNum;
            outer->placeType = new_outer_data.placeType;
            outer->roomType = new_outer_data.roomType;
            outer->limit = new_outer_data.limit;

            outer->outerItem->setText(0, new_outer_data.number);
            outer->calTotalPage->setInfo(outer->outerItem->parent()->parent()->text(0), new_outer_data.deck,
                                        new_outer_data.number, QString::number(new_outer_data.limit),
                                        QString::number(new_outer_data.ductNum));

            break;
        }
    }

    if(ducts.contains(old_outer_data.number)) {
        QList<Duct*> ductsToMove = ducts.value(old_outer_data.number);
        for(auto& duct : ductsToMove) {
            duct->ductCalPage->setInfo(duct->ductItem->parent()->parent()->parent()->text(0),
                                       new_outer_data.deck,
                                       new_outer_data.number);
        }
        ducts.remove(old_outer_data.number);
        ducts.insert(new_outer_data.number, ductsToMove);
    }
}

void RoomCalInfoManager::createDuct(bool isOuter, QString room_or_outer_number, QString ductNumber, int airVolume, bool isNamed)
{
    if (isOuter) {
        Outer* outer = nullptr;
        bool found = false;
        for (auto& outerList : outers) { // 假设你有一个类似的容器来存储所有室外对象
            for (auto& findOuter : outerList) {
                if (findOuter->number == room_or_outer_number) {
                    outer = findOuter;
                    found = true;
                    break;
                }
            }
            if (found) break;
        }

        if (!outer) {
            qDebug() << "outer is not found";
            return;
        }

        if(!outer->calTotalPage && !outer->noiSumItem && outer->ductNum == 0) {
            //室外噪音总和界面
            outer->calTotalPage = new Room_cal_total_widget;    //室外噪音总和界面
            outer->calTotalPage->setInfo(outer->outerItem->parent()->parent()->text(0), outer->deck, outer->number, QString::number(outer->limit), QString::number(outer->ductNum));
            stackedWidget->addWidget(outer->calTotalPage);

            outer->calTotalPage->change_outer_cal();
            outer->noiSumItem = new QTreeWidgetItem(outer->outerItem, QStringList("室外噪音"));

            connect(treeWidget,&QTreeWidget::itemClicked,this,[=](QTreeWidgetItem *item, int n) {
                if(item == outer->noiSumItem)
                    stackedWidget->setCurrentWidget(outer->calTotalPage);
            });
        }
        //如果主风管数量为1, 那么添加汇总前和汇总后界面
        else if(!outer->convergeBeforePage && !outer->convergeBeforeItem
                 && !outer->convergeAfterPage && !outer->convergeAfterItem
                 && outer->ductNum == 1) {
            //  保证房间噪音一直在最后一行
            int insertIndex = outer->outerItem->childCount() - 1;
            //汇合前叠加界面
            outer->convergeBeforeItem = new QTreeWidgetItem(QStringList("汇合前叠加"));
            outer->outerItem->insertChild(insertIndex, outer->convergeBeforeItem);
            outer->convergeBeforePage = new Outer_before_widget;
            outer->convergeBeforePage->setInfo(outer->outerItem->parent()->parent()->text(0), outer->deck, outer->number, QString::number(outer->limit), QString::number(outer->ductNum));
            //汇合后总管界面
            outer->convergeAfterItem = new QTreeWidgetItem(QStringList("汇合后总管"));
            outer->outerItem->insertChild(++insertIndex, outer->convergeAfterItem);
            outer->convergeAfterPage = new Outer_after_widget;
            outer->convergeAfterPage->setInfo(outer->outerItem->parent()->parent()->text(0), outer->deck, outer->number, QString::number(outer->limit), QString::number(outer->ductNum));
            stackedWidget->addWidget(outer->convergeBeforePage);
            stackedWidget->addWidget(outer->convergeAfterPage);

            connect(treeWidget,&QTreeWidget::itemClicked,this,[=](QTreeWidgetItem *item, int n) {
                if(item == outer->convergeBeforeItem)
                    stackedWidget->setCurrentWidget(outer->convergeBeforePage);
                else if(item == outer->convergeAfterItem)
                    stackedWidget->setCurrentWidget(outer->convergeAfterPage);
            });
        }

        outer->ductNum++;

        QTreeWidgetItem* ductItem = new QTreeWidgetItem(QStringList(ductNumber));

        //  保证房间噪音一直在最后一行
        int n = outer->outerItem->childCount();
        int insertIndex = qMax(0, n - 1);
        if(outer->ductNum >= 2) {
            insertIndex = n - 3;
        }
        outer->outerItem->insertChild(insertIndex, ductItem);

        Room_cal_baseWidget* ductCalPage = new Room_cal_baseWidget(nullptr, isOuter, isNamed);   //风管计算界面
        ductCalPage->set_cal_total_page(outer->calTotalPage);
        Duct* duct = new Duct{ductNumber, airVolume, ductItem, ductCalPage};
        ductCalPage->setInfo(duct->getParentMVZName(), outer->deck, outer->number);
        ductCalPage->setSystemName(outer->outerItem->parent()->text(0));
        if(isNamed) ductCalPage->setDuctNumber(duct->number);
        stackedWidget->addWidget(ductCalPage);
        ducts[outer->number].append(duct);
        connect(treeWidget, &QTreeWidget::itemClicked, this, [=](QTreeWidgetItem *item, int n) mutable {
            if(item == ductItem)
            {
                if(!ductCalPage->isNamed)
                {
                    Dialog_set_name* dialog = new Dialog_set_name("噪音支气管编号");
                    QString old_number = duct->number;
                    if(dialog->exec() == QDialog::Accepted)
                    {
                        duct->number = dialog->getName();
                        if(allNamesSet.find(duct->number) != allNamesSet.end()) {
                            QMessageBox::critical(nullptr, "命名重复", "噪音支气管 \'" + duct->number + "\' 已经存在");
                            return;
                        } else {
                            allNamesSet.insert(duct->number);
                        }

                        ductItem->setText(0, duct->number);
                        ductCalPage->setDuctNumber(dialog->getName());    //room_cal_table设置名称
                    } else {
                        emit switchWhitePage();
                        return;
                    }
                    ductCalPage->isNamed = true;
                    DatabaseManager::getInstance().namedDuct(true, old_number, const_cast<const Duct*&>(duct));
                }
                stackedWidget->setCurrentWidget(ductCalPage);
            }
        });
        if(!loadDataMode)
            DatabaseManager::getInstance().addDuctToDatabase(isOuter, const_cast<const Duct*&>(duct));

        if(outer->convergeBeforePage && outer->convergeAfterPage)
        {
            outer->convergeBeforePage->setDuctNum(QString::number(outer->ductNum));
            outer->convergeAfterPage->setDuctNum(QString::number(outer->ductNum));
        }

        System* system = nullptr;
        QString MVZName = duct->ductItem->parent()->parent()->parent()->text(0);
        QString systemName = duct->ductItem->parent()->parent()->text(0);
        for(auto& findSystem : systems[MVZName]) {
            if(findSystem->name == systemName) {
                system = findSystem;
                break;
            }
        }

        if(!system) {
            qDebug() << "system is not found";
            return;
        }

        system->roomDefinePage->setTableRowDuctNum(outer->number, QString::number(outer->ductNum));
    } else {
        Room* room = nullptr;

        bool found = false;
        for (auto& roomList : rooms) {
            for (auto& findRoom : roomList) {
                if (findRoom->number == room_or_outer_number) {
                    room = findRoom;
                    found = true;
                    break;
                }
            }
            if (found) break;
        }

        if (!room) {
            qDebug() << "room is not found";
            return;
        }

        // 如果主风管数量为0, 就先创建噪音汇总界面
        if (!room->calTotalPage && !room->noiSumItem && room->ductNum == 0) {
            // 室外噪音总和界面
            room->calTotalPage = new Room_cal_total_widget;
            room->calTotalPage->setInfo(room->roomItem->parent()->parent()->text(0), room->deck, room->number, QString::number(room->limit), QString::number(room->ductNum));
            stackedWidget->addWidget(room->calTotalPage);
            room->noiSumItem = new QTreeWidgetItem(room->roomItem, QStringList("房间噪音"));

            connect(treeWidget, &QTreeWidget::itemClicked, this, [=](QTreeWidgetItem* item, int n) {
                if (item == room->noiSumItem)
                    stackedWidget->setCurrentWidget(room->calTotalPage);
            });
        }

        if (!room->calTotalPage) {
            qDebug() << "calTotalPage is not found";
            return;
        }

        // 创建好calTotalPage后 主风管数量+1
        room->ductNum++;

        QTreeWidgetItem* ductItem = new QTreeWidgetItem(QStringList(ductNumber));

        //  保证房间噪音一直在最后一行
        int n = room->roomItem->childCount();
        int insertIndex = qMax(0, n - 1);
        room->roomItem->insertChild(insertIndex, ductItem);

        Room_cal_baseWidget* ductCalPage = new Room_cal_baseWidget(nullptr, isOuter, isNamed);
        ductCalPage->set_cal_total_page(room->calTotalPage);
        Duct* duct = new Duct{ductNumber, airVolume, ductItem, ductCalPage};
        ductCalPage->setInfo(room->roomItem->parent()->parent()->text(0), room->deck, room->number);
        ductCalPage->setSystemName(room->roomItem->parent()->text(0));
        if(isNamed)
            ductCalPage->setDuctNumber(duct->number);
        stackedWidget->addWidget(ductCalPage);
        ducts[room->number].append(duct);
        connect(treeWidget, &QTreeWidget::itemClicked, this, [=](QTreeWidgetItem* item, int n) mutable {
            if (item == ductItem) {
                if (!ductCalPage->isNamed) {
                    Dialog_set_name* dialog = new Dialog_set_name("主风管编号");
                    QString old_number = duct->number;
                    if (dialog->exec() == QDialog::Accepted) {
                        duct->number = dialog->getName();
                        if(allNamesSet.find(duct->number) != allNamesSet.end()) {
                            QMessageBox::critical(nullptr, "命名重复", "主风管 \'" + duct->number + "\' 已经存在");
                            return;
                        } else {
                            allNamesSet.insert(duct->number);
                        }

                        ductItem->setText(0, duct->number);
                        ductCalPage->setDuctNumber(dialog->getName());
                    } else {
                        emit switchWhitePage();
                        return;
                    }
                    ductCalPage->isNamed = true;
                    DatabaseManager::getInstance().namedDuct(false, old_number, const_cast<const Duct*&>(duct));
                }
                stackedWidget->setCurrentWidget(ductCalPage);
            }
        });

        if(!loadDataMode)
            DatabaseManager::getInstance().addDuctToDatabase(isOuter, const_cast<const Duct*&>(duct));

        room->calTotalPage->setDuctNum(QString::number(room->ductNum));

        System* system = nullptr;
        QString MVZName = duct->ductItem->parent()->parent()->parent()->text(0);
        QString systemName = duct->ductItem->parent()->parent()->text(0);
        for (auto& findSystem : systems[MVZName]) {
            if (findSystem->name == systemName) {
                system = findSystem;
                break;
            }
        }

        if (!system) {
            qDebug() << "system is not found";
            return;
        }

        system->roomDefinePage->setTableRowDuctNum(room->number, QString::number(room->ductNum));
    }
}

void RoomCalInfoManager::deleteDuct(QString number)
{
    Duct* duct = nullptr;
    bool found = false;
    for(auto& ductList : ducts) {
        for(auto& findDuct : ductList) {
            if(findDuct->number == number) {
                duct = findDuct;
                found = true;
                break;
            }
        }
        if(found) {
            break;
        }
    }

    if(!duct) {
        qDebug() << "duct is not found";
        return;
    }

    allNamesSet.remove(number);

    //先获得房间名或室外名
    QString room_or_outer_number = duct->ductItem->parent()->text(0);
    System* system = nullptr;
    Room* room = nullptr;
    Outer* outer = nullptr;
    int ductNum = -1;

    for (auto& roomList : rooms) {
        auto it = std::find_if(roomList.begin(), roomList.end(), [&room_or_outer_number](Room* findRoom) {
            return findRoom->number == room_or_outer_number;
        });
        if (it != roomList.end()) {
            room = *it;
            break;
        }
    }

    for (auto& outerList : outers) {
        auto it = std::find_if(outerList.begin(), outerList.end(), [&room_or_outer_number](Outer* findOuter) {
            return findOuter->number == room_or_outer_number;
        });
        if (it != outerList.end()) {
            outer = *it;
            break;
        }
    }

    if(!room && !outer) {
        qDebug() << "room's or outer's number = " + room_or_outer_number + " is not found";
        return;
    }

    QString MVZName = duct->ductItem->parent()->parent()->parent()->text(0);
    QString systemName = duct->ductItem->parent()->parent()->text(0);
    for(auto& findSystem : systems[MVZName]) {
        if(findSystem->name == systemName) {
            system = findSystem;
            break;
        }
    }

    if(!system) {
        qDebug() << "system is not found";
        return;
    }

    if(room && !outer)
    {
        ductNum = --room->ductNum;    //将room的主风管数量-1
        room->calTotalPage->setDuctNum(QString::number(ductNum)); //给汇总界面设置主风管数量
        room->calTotalPage->handle_duct_remove(duct->number); //汇总界面删除对应风管
    }
    else if(outer && !room) {
        ductNum = --outer->ductNum;
        outer->calTotalPage->setDuctNum(QString::number(ductNum)); //给汇总界面设置主风管数量
        outer->calTotalPage->handle_duct_remove(duct->number); //汇总界面删除对应风管
        if(outer->ductNum >= 2)
        {
            outer->convergeBeforePage->setDuctNum(QString::number(ductNum));
            outer->convergeAfterPage->setDuctNum(QString::number(ductNum));
        } else {
            stackedWidget->removeWidget(outer->convergeBeforePage);
            delete outer->convergeBeforePage;
            outer->convergeBeforePage = nullptr;
            delete outer->convergeBeforeItem;
            outer->convergeBeforeItem = nullptr;
            stackedWidget->removeWidget(outer->convergeAfterPage);
            delete outer->convergeAfterPage;
            outer->convergeAfterPage = nullptr;
            delete outer->convergeAfterItem;
            outer->convergeAfterItem = nullptr;
        }
    } else {
        qDebug() << "error";
        return;
    }


    //删除容器中的以及page和item
    ducts[room_or_outer_number].removeOne(duct);

    DatabaseManager::getInstance().delDuctInDatabase(outer ? true : false, const_cast<const Duct*&>(duct));

    stackedWidget->removeWidget(duct->ductCalPage);
    delete duct->ductItem;
    duct->ductItem = nullptr;
    delete duct->ductCalPage;
    duct->ductCalPage = nullptr;
    delete duct;
    duct = nullptr;

    //如果没有风管了, 就移除汇总界面
    if(ducts[room_or_outer_number].size() == 0) {
        if(room && !outer)
        {
            //清除噪音汇总页面
            stackedWidget->removeWidget(room->calTotalPage);
            delete room->calTotalPage;
            room->calTotalPage = nullptr;
            //清除treeItem
            delete room->noiSumItem;
            room->noiSumItem = nullptr;
        } else if(outer && !room) {
            //清除噪音汇总页面
            stackedWidget->removeWidget(outer->calTotalPage);
            delete outer->calTotalPage;
            outer->calTotalPage = nullptr;
            //清除treeItem
            delete outer->noiSumItem;
            outer->noiSumItem = nullptr;
        }
    }

    if(ductNum == -1) {
        qDebug() << "ductNum wrong";
        return;
    }

    system->roomDefinePage->setTableRowDuctNum(room_or_outer_number, QString::number(ductNum));
}

