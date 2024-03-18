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

void ComponentManager::updateRevisedComponent(QString table_id, QSharedPointer<ComponentBase> component, QString type)
{
    //使用基类指针进行处理
    if (type == "布风器+散流器")
    {
        QSharedPointer<AirDiff_noise> input_component = qSharedPointerCast<AirDiff_noise>(component);
        auto it = airDiffs.begin();
        for ( ; it != airDiffs.end(); ++it) {
            // 使用QSharedPointer，可以直接解引用来访问对象的成员
            if (table_id == (*it)->table_id) {
                (*it) = input_component;  // 如果找到，替换
                break;
            }
        }
    }
    else if(type == "空调器")
    {
        QSharedPointer<Aircondition_noise> input_component = qSharedPointerCast<Aircondition_noise>(component);
        auto it = airConditions.begin();
        for ( ; it != airConditions.end(); ++it) {
            // 使用QSharedPointer，可以直接解引用来访问对象的成员
            if (table_id == (*it)->table_id) {
                (*it) = input_component;  // 如果找到，替换
                break;
            }
        }
    }
    else if(type == "圆形调风门")
    {
        QSharedPointer<Circular_damper_noi> input_component = qSharedPointerCast<Circular_damper_noi>(component);
        auto it = circularDampers.begin();
        for ( ; it != circularDampers.end(); ++it) {
            // 使用QSharedPointer，可以直接解引用来访问对象的成员
            if (table_id == (*it)->table_id) {
                (*it) = input_component;  // 如果找到，替换
                break;
            }
        }
    }
    else if(type == "置换通风末端")
    {
        QSharedPointer<Disp_vent_terminal_noise> input_component = qSharedPointerCast<Disp_vent_terminal_noise>(component);
        auto it = dispVentTerminals.begin();
        for ( ; it != dispVentTerminals.end(); ++it) {
            // 使用QSharedPointer，可以直接解引用来访问对象的成员
            if (table_id == (*it)->table_id) {
                (*it) = input_component;  // 如果找到，替换
                break;
            }
        }
    }
    else if(type == "风道多分支")
    {
        QSharedPointer<Multi_ranc_atten> input_component = qSharedPointerCast<Multi_ranc_atten>(component);
        auto it = multiRancs.begin();
        for ( ; it != multiRancs.end(); ++it) {
            // 使用QSharedPointer，可以直接解引用来访问对象的成员
            if (table_id == (*it)->table_id) {
                (*it) = input_component;  // 如果找到，替换
                break;
            }
        }
    }
    else if(type == "弯头")
    {
        QSharedPointer<Elbow_atten> input_component = qSharedPointerCast<Elbow_atten>(component);
        auto it = elbows.begin();
        for ( ; it != elbows.end(); ++it) {
            // 使用QSharedPointer，可以直接解引用来访问对象的成员
            if (table_id == (*it)->table_id) {
                (*it) = input_component;  // 如果找到，替换
                break;
            }
        }
    }
    else if(type == "风机")
    {
        QSharedPointer<Fan_noise> input_component = qSharedPointerCast<Fan_noise>(component);
        auto it = fans.begin();
        for ( ; it != fans.end(); ++it) {
            // 使用QSharedPointer，可以直接解引用来访问对象的成员
            if (table_id == (*it)->table_id) {
                (*it) = input_component;  // 如果找到，替换
                break;
            }
        }
    }
    else if(type == "风机盘管")
    {
        QSharedPointer<FanCoil_noise> input_component = qSharedPointerCast<FanCoil_noise>(component);
        auto it = fanCoils.begin();
        for ( ; it != fanCoils.end(); ++it) {
            // 使用QSharedPointer，可以直接解引用来访问对象的成员
            if (table_id == (*it)->table_id) {
                (*it) = input_component;  // 如果找到，替换
                break;
            }
        }
    }
    else if(type == "其他送风末端")
    {
        QSharedPointer<Other_send_terminal_noise> input_component = qSharedPointerCast<Other_send_terminal_noise>(component);
        auto it = otherSendTerminals.begin();
        for ( ; it != otherSendTerminals.end(); ++it) {
            // 使用QSharedPointer，可以直接解引用来访问对象的成员
            if (table_id == (*it)->table_id) {
                (*it) = input_component;  // 如果找到，替换
                break;
            }
        }
    }
    else if(type == "直管")
    {
        QSharedPointer<Pipe_atten> input_component = qSharedPointerCast<Pipe_atten>(component);
        auto it = pipes.begin();
        for ( ; it != pipes.end(); ++it) {
            // 使用QSharedPointer，可以直接解引用来访问对象的成员
            if (table_id == (*it)->table_id) {
                (*it) = input_component;  // 如果找到，替换
                break;
            }
        }
    }
    else if(type == "抽/送风头")
    {
        QSharedPointer<PumpSend_noise> input_component = qSharedPointerCast<PumpSend_noise>(component);
        auto it = pumpSends.begin();
        for ( ; it != pumpSends.end(); ++it) {
            // 使用QSharedPointer，可以直接解引用来访问对象的成员
            if (table_id == (*it)->table_id) {
                (*it) = input_component;  // 如果找到，替换
                break;
            }
        }
    }
    else if(type == "方形调风门")
    {
        QSharedPointer<Rect_damper_noi> input_component = qSharedPointerCast<Rect_damper_noi>(component);
        auto it = rectDampers.begin();
        for ( ; it != rectDampers.end(); ++it) {
            // 使用QSharedPointer，可以直接解引用来访问对象的成员
            if (table_id == (*it)->table_id) {
                (*it) = input_component;  // 如果找到，替换
                break;
            }
        }
    }
    else if(type == "变径")
    {
        QSharedPointer<Reducer_atten> input_component = qSharedPointerCast<Reducer_atten>(component);
        auto it = reducers.begin();
        for ( ; it != reducers.end(); ++it) {
            // 使用QSharedPointer，可以直接解引用来访问对象的成员
            if (table_id == (*it)->table_id) {
                (*it) = input_component;  // 如果找到，替换
                break;
            }
        }
    }
    else if(type == "静压箱+格栅")
    {
        QSharedPointer<Reducer_atten> input_component = qSharedPointerCast<Reducer_atten>(component);
        auto it = reducers.begin();
        for ( ; it != reducers.end(); ++it) {
            // 使用QSharedPointer，可以直接解引用来访问对象的成员
            if (table_id == (*it)->table_id) {
                (*it) = input_component;  // 如果找到，替换
                break;
            }
        }
    }
    else if(type == "消声器")
    {
        QSharedPointer<Silencer_atten> input_component = qSharedPointerCast<Silencer_atten>(component);
        auto it = silencers.begin();
        for ( ; it != silencers.end(); ++it) {
            // 使用QSharedPointer，可以直接解引用来访问对象的成员
            if (table_id == (*it)->table_id) {
                (*it) = input_component;  // 如果找到，替换
                break;
            }
        }
    }
    else if(type == "静压箱")
    {
        QSharedPointer<Static_box> input_component = qSharedPointerCast<Static_box>(component);
        auto it = staticboxs.begin();
        for ( ; it != staticboxs.end(); ++it) {
            // 使用QSharedPointer，可以直接解引用来访问对象的成员
            if (table_id == (*it)->table_id) {
                (*it) = input_component;  // 如果找到，替换
                break;
            }
        }
    }
    else if(type == "三通")
    {
        QSharedPointer<Tee_atten> input_component = qSharedPointerCast<Tee_atten>(component);
        auto it = tees.begin();
        for ( ; it != tees.end(); ++it) {
            // 使用QSharedPointer，可以直接解引用来访问对象的成员
            if (table_id == (*it)->table_id) {
                (*it) = input_component;  // 如果找到，替换
                break;
            }
        }
    }
    else if(type == "变风量末端")
    {
        QSharedPointer<VAV_terminal_noise> input_component = qSharedPointerCast<VAV_terminal_noise>(component);
        auto it = VAVTerminals.begin();
        for ( ; it != VAVTerminals.end(); ++it) {
            // 使用QSharedPointer，可以直接解引用来访问对象的成员
            if (table_id == (*it)->table_id) {
                (*it) = input_component;  // 如果找到，替换
                break;
            }
        }
    }
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
    if(number == "")
        return nullptr;

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

    return nullptr;
}

ComponentBase* ComponentManager::getComponentByModel(QString model, QString type)
{
    if(model == "")
        return nullptr;

    if(type == "风机盘管")
    {
        for (const QSharedPointer<FanCoil_noise>& fanCoil : fanCoils) {
            if (fanCoil->model == model) {
                return fanCoil.data(); // 返回匹配的型号
            }
        }
    }

    return nullptr;
}

QList<const ComponentBase*> ComponentManager::getComponents(QString type)
{
    QList<const ComponentBase*> result;

    if(type == "变风量末端")
    {
        for (const QSharedPointer<VAV_terminal_noise>& component : VAVTerminals) {
            result.push_back(component.get());
        }
    }
    else if(type == "圆形调风门")
    {
        for (const QSharedPointer<Circular_damper_noi>& component : circularDampers)
        {
            result.push_back(component.get());
        }
    }
    else if(type == "方形调风门")
    {
        for (const QSharedPointer<Rect_damper_noi>& component : rectDampers)
        {
            result.push_back(component.get());
        }
    }
    else if(type == "布风器+散流器")
    {
        for (const QSharedPointer<AirDiff_noise>& component : airDiffs)
        {
            result.push_back(component.get());
        }
    }

    return result;
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
