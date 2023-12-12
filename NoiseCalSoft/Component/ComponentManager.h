#ifndef COMPONENTMANAGER_H
#define COMPONENTMANAGER_H

#include <unordered_map>
#include <QString>
#include <QList>
#include <QSharedPointer>
#include <QtGlobal> // 确保包含了 QtGlobal 头文件
#include <QDebug>
#include "Component/ComponentStructs.h"

class ComponentManager
{
public:
    static ComponentManager& getInstance() {
        static ComponentManager instance; // 在首次调用时创建唯一实例
        return instance;
    }

    // 添加 AirDiff_noise 部件
    void addAirDiff(QSharedPointer<AirDiff_noise> airDiff) { airDiffs.append(airDiff); };

    // 添加 Aircondition_noise 部件
    void addAirCondition(QSharedPointer<Aircondition_noise> airCondition){ airConditions.append(airCondition); };

    // 添加 Circular_damper_noi 部件
    void addCircularDamper(QSharedPointer<Circular_damper_noi> circularDamper){ circularDampers.append(circularDamper); };

    // 添加 Disp_vent_terminal_noise 部件
    void addDispVentTerminal(QSharedPointer<Disp_vent_terminal_noise> dispVentTerminal){ dispVentTerminals.append(dispVentTerminal); };

    // 添加 Multi_ranc_atten 部件
    void addMultiRanc(QSharedPointer<Multi_ranc_atten> multiRanc){ multiRancs.append(multiRanc); };

    // 添加 Elbow_atten 部件
    void addElbow(QSharedPointer<Elbow_atten> elbow){ elbows.append(elbow); };

    // 添加 Fan_noise 部件
    void addFan(QSharedPointer<Fan_noise> fan){
        fans.append(fan);
        for(auto it : fans)
        {
            qDebug() << "table_id : " << (*it).table_id << " number : " << (*it).number;
        }
    };

    // 添加 FanCoil_noise 部件
    void addFanCoil(QSharedPointer<FanCoil_noise> fanCoil){ fanCoils.append(fanCoil); };

    // 添加 Other_send_terminal_noise 部件
    void addOtherSendTerminal(QSharedPointer<Other_send_terminal_noise> otherSendTerminal){ otherSendTerminals.append(otherSendTerminal); };

    // 添加 Pipe_atten 部件
    void addPipe(QSharedPointer<Pipe_atten> pipe){ pipes.append(pipe); };

    // 添加 PumpSend_noise 部件
    void addPumpSend(QSharedPointer<PumpSend_noise> pumpSend){ pumpSends.append(pumpSend); };

    // 添加 Rect_damper_noi 部件
    void addRectDamper(QSharedPointer<Rect_damper_noi> rectDamper){ rectDampers.append(rectDamper); };

    // 添加 Reducer_atten 部件
    void addReducer(QSharedPointer<Reducer_atten> reducer){ reducers.append(reducer); };

    // 添加 Static_box_grille_noise 部件
    void addStaticBoxGrille(QSharedPointer<StaticBox_grille_noise> staticBoxGrille){ staticBoxGrilles.append(staticBoxGrille); };

    // 添加 Silencer_atten 部件
    void addSilencer(QSharedPointer<Silencer_atten> silencer){ silencers.append(silencer); };

    // 添加 Static_box 部件
    void addStaticBox(QSharedPointer<Static_box> staticbox){ staticboxs.append(staticbox); };

    // 添加 Tee_atten 部件
    void addTee(QSharedPointer<Tee_atten> tee){ tees.append(tee); };

    // 添加 VAV_terminal_noise 部件
    void addVAVTerminal(QSharedPointer<VAV_terminal_noise> VAVTerminal){ VAVTerminals.append(VAVTerminal); };

    void del_and_updateTableID(int deleteID, QString containerName);

private:
    // 禁止通过构造函数和复制构造函数创建实例
    ComponentManager();
    ComponentManager(const ComponentManager&) = delete;
    ComponentManager& operator=(const ComponentManager&) = delete;

    // 列表存储各种部件
    QList<QSharedPointer<AirDiff_noise>> airDiffs;
    QList<QSharedPointer<Aircondition_noise>> airConditions;
    QList<QSharedPointer<Circular_damper_noi>> circularDampers;
    QList<QSharedPointer<Disp_vent_terminal_noise>> dispVentTerminals;
    QList<QSharedPointer<Multi_ranc_atten>> multiRancs;
    QList<QSharedPointer<Elbow_atten>> elbows;
    QList<QSharedPointer<Fan_noise>> fans;
    QList<QSharedPointer<FanCoil_noise>> fanCoils;
    QList<QSharedPointer<Other_send_terminal_noise>> otherSendTerminals;
    QList<QSharedPointer<Pipe_atten>> pipes;
    QList<QSharedPointer<PumpSend_noise>> pumpSends;
    QList<QSharedPointer<Rect_damper_noi>> rectDampers;
    QList<QSharedPointer<Reducer_atten>> reducers;
    QList<QSharedPointer<StaticBox_grille_noise>> staticBoxGrilles;
    QList<QSharedPointer<Silencer_atten>> silencers;
    QList<QSharedPointer<Static_box>> staticboxs;
    QList<QSharedPointer<Tee_atten>> tees;
    QList<QSharedPointer<VAV_terminal_noise>> VAVTerminals;
};

#endif // COMPONENTMANAGER_H
