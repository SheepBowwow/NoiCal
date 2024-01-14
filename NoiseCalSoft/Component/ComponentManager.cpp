#include "ComponentManager.h"
#include <QtGlobal>
#include <QSharedPointer>


ComponentManager::ComponentManager()
{

}

void ComponentManager::addComponent(QSharedPointer<ComponentBase> component)
{
    //使用基类指针进行处理
    if (qSharedPointerCast<AirDiff_noise>(component)) airDiffs.append(qSharedPointerCast<AirDiff_noise>(component));
    else if(qSharedPointerCast<Aircondition_noise>(component)) airConditions.append(qSharedPointerCast<Aircondition_noise>(component));
    else if(qSharedPointerCast<Circular_damper_noi>(component)) circularDampers.append(qSharedPointerCast<Circular_damper_noi>(component));
    else if(qSharedPointerCast<Disp_vent_terminal_noise>(component)) dispVentTerminals.append(qSharedPointerCast<Disp_vent_terminal_noise>(component));
    else if(qSharedPointerCast<Multi_ranc_atten>(component)) multiRancs.append(qSharedPointerCast<Multi_ranc_atten>(component));
    else if(qSharedPointerCast<Elbow_atten>(component)) elbows.append(qSharedPointerCast<Elbow_atten>(component));
    else if(qSharedPointerCast<Fan_noise>(component)) fans.append(qSharedPointerCast<Fan_noise>(component));
    else if(qSharedPointerCast<FanCoil_noise>(component)) fanCoils.append(qSharedPointerCast<FanCoil_noise>(component));
    else if(qSharedPointerCast<Other_send_terminal_noise>(component)) otherSendTerminals.append(qSharedPointerCast<Other_send_terminal_noise>(component));
    else if(qSharedPointerCast<Pipe_atten>(component)) pipes.append(qSharedPointerCast<Pipe_atten>(component));
    else if(qSharedPointerCast<PumpSend_noise>(component)) pumpSends.append(qSharedPointerCast<PumpSend_noise>(component));
    else if(qSharedPointerCast<Rect_damper_noi>(component)) rectDampers.append(qSharedPointerCast<Rect_damper_noi>(component));
    else if(qSharedPointerCast<Reducer_atten>(component)) reducers.append(qSharedPointerCast<Reducer_atten>(component));
    else if(qSharedPointerCast<StaticBox_grille_noise>(component)) staticBoxGrilles.append(qSharedPointerCast<StaticBox_grille_noise>(component));
    else if(qSharedPointerCast<Silencer_atten>(component)) silencers.append(qSharedPointerCast<Silencer_atten>(component));
    else if(qSharedPointerCast<Static_box>(component)) staticboxs.append(qSharedPointerCast<Static_box>(component));
    else if(qSharedPointerCast<Tee_atten>(component)) tees.append(qSharedPointerCast<Tee_atten>(component));
    else if(qSharedPointerCast<VAV_terminal_noise>(component)) VAVTerminals.append(qSharedPointerCast<VAV_terminal_noise>(component));
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
