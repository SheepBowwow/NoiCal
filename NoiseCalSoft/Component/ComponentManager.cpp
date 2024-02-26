#include "ComponentManager.h"
#include <QtGlobal>
#include <QSharedPointer>


ComponentManager::ComponentManager()
{

}

QVector<QString> ComponentManager::getAirconditionNumbers()
{
    QVector<QString> AirconditionNumbers;
    // 遍历airConditions容器
    for (const QSharedPointer<Aircondition_noise>& airCondition : airConditions) {
        AirconditionNumbers.push_back(airCondition->number);
    }
    return AirconditionNumbers;
}

QVector<QString> ComponentManager::getFanCoilNumbers()
{
    QVector<QString> FanCoilNumbers;
    for (const QSharedPointer<FanCoil_noise>& fanCoil : fanCoils) {
        FanCoilNumbers.push_back(fanCoil->model);
    }
    return FanCoilNumbers;
}

QVector<QString> ComponentManager::getFanNumbers()
{
    QVector<QString> FanNumbers;
    for (const QSharedPointer<Fan_noise>& fan : fans) {
        FanNumbers.push_back(fan->number);
    }

    return FanNumbers;
}

void ComponentManager::addComponent(QSharedPointer<ComponentBase> component, QString type)
{

    //使用基类指针进行处理
    if (type == "布风器+散流器") airDiffs.append(qSharedPointerCast<AirDiff_noise>(component));
    else if(type == "空调器") airConditions.append(qSharedPointerCast<Aircondition_noise>(component));
    else if(type == "圆形调风门") circularDampers.append(qSharedPointerCast<Circular_damper_noi>(component));
    else if(type == "置换通风末端") dispVentTerminals.append(qSharedPointerCast<Disp_vent_terminal_noise>(component));
    else if(type == "风道多分支") multiRancs.append(qSharedPointerCast<Multi_ranc_atten>(component));
    else if(type == "弯头") elbows.append(qSharedPointerCast<Elbow_atten>(component));
    else if(type == "风机") fans.append(qSharedPointerCast<Fan_noise>(component));
    else if(type == "风机盘管") fanCoils.append(qSharedPointerCast<FanCoil_noise>(component));
    else if(type == "其他送风末端") otherSendTerminals.append(qSharedPointerCast<Other_send_terminal_noise>(component));
    else if(type == "直管") pipes.append(qSharedPointerCast<Pipe_atten>(component));
    else if(type == "抽/送风头") pumpSends.append(qSharedPointerCast<PumpSend_noise>(component));
    else if(type == "方形调风门") rectDampers.append(qSharedPointerCast<Rect_damper_noi>(component));
    else if(type == "变径") reducers.append(qSharedPointerCast<Reducer_atten>(component));
    else if(type == "静压箱+格栅") staticBoxGrilles.append(qSharedPointerCast<StaticBox_grille_noise>(component));
    else if(type == "消声器") silencers.append(qSharedPointerCast<Silencer_atten>(component));
    else if(type == "静压箱") staticboxs.append(qSharedPointerCast<Static_box>(component));
    else if(type == "三通") tees.append(qSharedPointerCast<Tee_atten>(component));
    else if(type == "变风量末端") VAVTerminals.append(qSharedPointerCast<VAV_terminal_noise>(component));
}

QString ComponentManager::getModelByNumber(QString number)
{
    // 遍历airConditions容器
    for (const QSharedPointer<Aircondition_noise>& airCondition : airConditions) {
        // 检查空调编号是否匹配
        if (airCondition->number == number) {
            return airCondition->model; // 返回匹配的型号
        }
    }

    // 遍历fans容器
    for (const QSharedPointer<Fan_noise>& fan : fans) {
        // 检查空调编号是否匹配
        if (fan->number == number) {
            return fan->model; // 返回匹配的型号
        }
    }
    return QString(); // 如果未找到匹配项，返回空字符串
}

ComponentBase* ComponentManager::getComponentByNumber(QString number)
{
    // 遍历airConditions容器
    for (const QSharedPointer<Aircondition_noise>& airCondition : airConditions) {
        // 检查空调编号是否匹配
        if (airCondition->number == number) {
            return airCondition.data(); // 返回匹配的型号
        }
    }

    // 遍历fans容器
    for (const QSharedPointer<Fan_noise>& fan : fans) {
        // 检查空调编号是否匹配
        if (fan->number == number) {
            return fan.data(); // 返回匹配的型号
        }
    }

    // 遍历fans容器
    for (const QSharedPointer<FanCoil_noise>& fanCoil : fanCoils) {
        // 检查空调编号是否匹配
        if (fanCoil->model == number) {
            return fanCoil.data(); // 返回匹配的型号
        }
    }
}

template<typename T>
void ComponentManager::del_and_updateTableIDBase(QList<QSharedPointer<T>> list, int deleteID, QString containerName)
{
    // 删除 table_id 等于 deleteID 的元素
    auto it = std::remove_if(list.begin(), list.end(),
        [deleteID](const auto& component) {
            return component->table_id.toInt() == deleteID;
    });

    list.erase(it, list.end());

    for (auto& component : list) {
        if (component->table_id.toInt() > deleteID) {
            component->table_id = QString::number(component->table_id.toInt() - 1);
        }
    }
}

void ComponentManager::del_and_updateTableID(int deleteID, QString containerName)
{
    if(containerName == "布风器+散流器")
    {
        del_and_updateTableIDBase(airDiffs, deleteID, containerName);
    }
    else if(containerName == "空调器")
    {
        del_and_updateTableIDBase(airConditions, deleteID, containerName);
    }
    else if(containerName == "圆形调风门")
    {
        del_and_updateTableIDBase(circularDampers, deleteID, containerName);
    }
    else if(containerName == "置换通风末端")
    {
        del_and_updateTableIDBase(dispVentTerminals, deleteID, containerName);
    }
    else if(containerName == "风道多分支")
    {
        del_and_updateTableIDBase(multiRancs, deleteID, containerName);
    }
    else if(containerName == "弯头")
    {
        del_and_updateTableIDBase(elbows, deleteID, containerName);
    }
    else if(containerName == "风机")
    {
        del_and_updateTableIDBase(fans, deleteID, containerName);
    }
    else if(containerName == "风机盘管")
    {
        del_and_updateTableIDBase(fanCoils, deleteID, containerName);
    }
    else if(containerName == "其他送风末端")
    {
        del_and_updateTableIDBase(otherSendTerminals, deleteID, containerName);
    }
    else if(containerName == "直管")
    {
        del_and_updateTableIDBase(pipes, deleteID, containerName);
    }
    else if(containerName == "抽/送风头")
    {
        del_and_updateTableIDBase(pumpSends, deleteID, containerName);
    }
    else if(containerName == "方形调风门")
    {
        del_and_updateTableIDBase(rectDampers, deleteID, containerName);
    }
    else if(containerName == "变径")
    {
        del_and_updateTableIDBase(reducers, deleteID, containerName);
    }
    else if(containerName == "静压箱+格栅")
    {
        del_and_updateTableIDBase(staticBoxGrilles, deleteID, containerName);
    }
    else if(containerName == "消声器")
    {
        del_and_updateTableIDBase(silencers, deleteID, containerName);
    }
    else if(containerName == "静压箱")
    {
        del_and_updateTableIDBase(staticboxs, deleteID, containerName);
    }
    else if(containerName == "三通")
    {
        del_and_updateTableIDBase(tees, deleteID, containerName);
    }
    else if(containerName == "变风量末端")
    {
        del_and_updateTableIDBase(VAVTerminals, deleteID, containerName);
    }
}
