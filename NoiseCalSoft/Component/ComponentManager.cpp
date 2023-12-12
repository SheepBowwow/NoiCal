#include "ComponentManager.h"
#include <QtGlobal>

ComponentManager::ComponentManager()
{

}

void ComponentManager::del_and_updateTableID(int deleteID, QString containerName)
{
    if(containerName == "布风器+散流器")
    {
        // 删除 table_id 等于 deleteID 的元素
        auto it = std::remove_if(airDiffs.begin(), airDiffs.end(),
            [deleteID](const auto& component) {
                return component->table_id.toInt() == deleteID;
        });

        airDiffs.erase(it, airDiffs.end());

        for (auto& component : airDiffs) {
            if (component->table_id.toInt() > deleteID) {
                component->table_id = QString::number(component->table_id.toInt() - 1);
            }
        }
    }
    else if(containerName == "空调器")
    {
        // 删除 table_id 等于 deleteID 的元素
        auto it = std::remove_if(airConditions.begin(), airConditions.end(),
            [deleteID](const auto& component) {
                return component->table_id.toInt() == deleteID;
        });

        airConditions.erase(it, airConditions.end());

        for (auto& component : airConditions) {
            if (component->table_id.toInt() > deleteID) {
                component->table_id = QString::number(component->table_id.toInt() - 1);
            }
        }
    }
    else if(containerName == "圆形调风门")
    {
        // 删除 table_id 等于 deleteID 的元素
        auto it = std::remove_if(circularDampers.begin(), circularDampers.end(),
            [deleteID](const auto& component) {
                return component->table_id.toInt() == deleteID;
        });

        circularDampers.erase(it, circularDampers.end());

        for (auto& component : circularDampers) {
            if (component->table_id.toInt() > deleteID) {
                component->table_id = QString::number(component->table_id.toInt() - 1);
            }
        }
    }
    else if(containerName == "置换通风末端")
    {
        // 删除 table_id 等于 deleteID 的元素
        auto it = std::remove_if(dispVentTerminals.begin(), dispVentTerminals.end(),
            [deleteID](const auto& component) {
                return component->table_id.toInt() == deleteID;
        });

        dispVentTerminals.erase(it, dispVentTerminals.end());

        for (auto& component : dispVentTerminals) {
            if (component->table_id.toInt() > deleteID) {
                component->table_id = QString::number(component->table_id.toInt() - 1);
            }
        }
    }
    else if(containerName == "风道多分支")
    {
        // 删除 table_id 等于 deleteID 的元素
        auto it = std::remove_if(multiRancs.begin(), multiRancs.end(),
            [deleteID](const auto& component) {
                return component->table_id.toInt() == deleteID;
        });

        multiRancs.erase(it, multiRancs.end());

        for (auto& component : multiRancs) {
            if (component->table_id.toInt() > deleteID) {
                component->table_id = QString::number(component->table_id.toInt() - 1);
            }
        }
    }
    else if(containerName == "弯头")
    {
        // 删除 table_id 等于 deleteID 的元素
        auto it = std::remove_if(elbows.begin(), elbows.end(),
            [deleteID](const auto& component) {
                return component->table_id.toInt() == deleteID;
        });

        elbows.erase(it, elbows.end());

        for (auto& component : elbows) {
            if (component->table_id.toInt() > deleteID) {
                component->table_id = QString::number(component->table_id.toInt() - 1);
            }
        }
    }
    else if(containerName == "风机")
    {
        // 删除 table_id 等于 deleteID 的元素
        auto it = std::remove_if(fans.begin(), fans.end(),
            [deleteID](const auto& component) {
                return component->table_id.toInt() == deleteID;
        });

        fans.erase(it, fans.end());


        for (auto& component : fans) {
            if (component->table_id.toInt() > deleteID) {
                component->table_id = QString::number(component->table_id.toInt() - 1);
            }
        }
    }
    else if(containerName == "风机盘管")
    {
        // 删除 table_id 等于 deleteID 的元素
        auto it = std::remove_if(fanCoils.begin(), fanCoils.end(),
            [deleteID](const auto& component) {
                return component->table_id.toInt() == deleteID;
        });

        fanCoils.erase(it, fanCoils.end());

        for (auto& component : fanCoils) {
            if (component->table_id.toInt() > deleteID) {
                component->table_id = QString::number(component->table_id.toInt() - 1);
            }
        }
    }
    else if(containerName == "其他送风末端")
    {
        // 删除 table_id 等于 deleteID 的元素
        auto it = std::remove_if(otherSendTerminals.begin(), otherSendTerminals.end(),
            [deleteID](const auto& component) {
                return component->table_id.toInt() == deleteID;
        });

        otherSendTerminals.erase(it, otherSendTerminals.end());

        for (auto& component : otherSendTerminals) {
            if (component->table_id.toInt() > deleteID) {
                component->table_id = QString::number(component->table_id.toInt() - 1);
            }
        }
    }
    else if(containerName == "直管")
    {
        // 删除 table_id 等于 deleteID 的元素
        auto it = std::remove_if(pipes.begin(), pipes.end(),
            [deleteID](const auto& component) {
                return component->table_id.toInt() == deleteID;
        });

        pipes.erase(it, pipes.end());

        for (auto& component : pipes) {
            if (component->table_id.toInt() > deleteID) {
                component->table_id = QString::number(component->table_id.toInt() - 1);
            }
        }
    }
    else if(containerName == "抽/送风头")
    {
        // 删除 table_id 等于 deleteID 的元素
        auto it = std::remove_if(pumpSends.begin(), pumpSends.end(),
            [deleteID](const auto& component) {
                return component->table_id.toInt() == deleteID;
        });

        pumpSends.erase(it, pumpSends.end());

        for (auto& component : pumpSends) {
            if (component->table_id.toInt() > deleteID) {
                component->table_id = QString::number(component->table_id.toInt() - 1);
            }
        }
    }
    else if(containerName == "方形调风门")
    {
        // 删除 table_id 等于 deleteID 的元素
        auto it = std::remove_if(rectDampers.begin(), rectDampers.end(),
            [deleteID](const auto& component) {
                return component->table_id.toInt() == deleteID;
        });

        rectDampers.erase(it, rectDampers.end());

        for (auto& component : rectDampers) {
            if (component->table_id.toInt() > deleteID) {
                component->table_id = QString::number(component->table_id.toInt() - 1);
            }
        }
    }
    else if(containerName == "变径")
    {
        // 删除 table_id 等于 deleteID 的元素
        auto it = std::remove_if(reducers.begin(), reducers.end(),
            [deleteID](const auto& component) {
                return component->table_id.toInt() == deleteID;
        });

        reducers.erase(it, reducers.end());

        for (auto& component : reducers) {
            if (component->table_id.toInt() > deleteID) {
                component->table_id = QString::number(component->table_id.toInt() - 1);
            }
        }
    }
    else if(containerName == "静压箱+格栅")
    {
        // 删除 table_id 等于 deleteID 的元素
        auto it = std::remove_if(staticBoxGrilles.begin(), staticBoxGrilles.end(),
            [deleteID](const auto& component) {
                return component->table_id.toInt() == deleteID;
        });

        staticBoxGrilles.erase(it, staticBoxGrilles.end());

        for (auto& component : staticBoxGrilles) {
            if (component->table_id.toInt() > deleteID) {
                component->table_id = QString::number(component->table_id.toInt() - 1);
            }
        }
    }
    else if(containerName == "消声器")
    {
        // 删除 table_id 等于 deleteID 的元素
        auto it = std::remove_if(silencers.begin(), silencers.end(),
            [deleteID](const auto& component) {
                return component->table_id.toInt() == deleteID;
        });

        silencers.erase(it, silencers.end());

        for (auto& component : silencers) {
            if (component->table_id.toInt() > deleteID) {
                component->table_id = QString::number(component->table_id.toInt() - 1);
            }
        }
    }
    else if(containerName == "静压箱")
    {
        // 删除 table_id 等于 deleteID 的元素
        auto it = std::remove_if(staticboxs.begin(), staticboxs.end(),
            [deleteID](const auto& component) {
                return component->table_id.toInt() == deleteID;
        });

        staticboxs.erase(it, staticboxs.end());

        for (auto& component : staticboxs) {
            if (component->table_id.toInt() > deleteID) {
                component->table_id = QString::number(component->table_id.toInt() - 1);
            }
        }
    }
    else if(containerName == "三通")
    {
        // 删除 table_id 等于 deleteID 的元素
        auto it = std::remove_if(tees.begin(), tees.end(),
            [deleteID](const auto& component) {
                return component->table_id.toInt() == deleteID;
        });

        tees.erase(it, tees.end());

        for (auto& component : tees) {
            if (component->table_id.toInt() > deleteID) {
                component->table_id = QString::number(component->table_id.toInt() - 1);
            }
        }
    }
    else if(containerName == "变风量末端")
    {
        // 删除 table_id 等于 deleteID 的元素
        auto it = std::remove_if(VAVTerminals.begin(), VAVTerminals.end(),
            [deleteID](const auto& component) {
                return component->table_id.toInt() == deleteID;
        });

        VAVTerminals.erase(it, VAVTerminals.end());

        for (auto& component : VAVTerminals) {
            if (component->table_id.toInt() > deleteID) {
                component->table_id = QString::number(component->table_id.toInt() - 1);
            }
        }
    }
}
