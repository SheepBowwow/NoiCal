#ifndef COMPONENTMANAGER_H
#define COMPONENTMANAGER_H

#include <QList>
#include <QSharedPointer>
#include "Component/ComponentStructs.h"

class ComponentManager
{
public:
    static ComponentManager& getInstance() {
        static ComponentManager instance; // 在首次调用时创建唯一实例
        return instance;
    }

    // 列表存储各种部件
    QList<QSharedPointer<AirDiff_noise>> airDiff;
    QList<QSharedPointer<Aircondition_noise>> airCondition;
    QList<QSharedPointer<Circular_damper_noi>> circularDamper;
    QList<QSharedPointer<Disp_vent_terminal_noise>> dispVentTerminal;
    QList<QSharedPointer<Multi_ranc_atten>> multiRanc;
    QList<QSharedPointer<Elbow_atten>> elbow;
    QList<QSharedPointer<Fan_noise>> fan;
    QList<QSharedPointer<FanCoil_noise>> fanCoil;
    QList<QSharedPointer<Other_send_terminal_noise>> otherSendTerminal;
    QList<QSharedPointer<Pipe_atten>> pipe;
    QList<QSharedPointer<PumpSend_noise>> pumpSend;
    QList<QSharedPointer<Rect_damper_noi>> rectDamper;
    QList<QSharedPointer<Reducer_atten>> reducer;
    QList<QSharedPointer<StaticBox_grille_noise>> staticBoxGrille;
    QList<QSharedPointer<Silencer_atten>> silencer;
    QList<QSharedPointer<Static_box>> staticbox;
    QList<QSharedPointer<Tee_atten>> tee;
    QList<QSharedPointer<VAV_terminal_noise>> VAVTerminal;

    // 添加 AirDiff_noise 部件
    void addAirDiff(QSharedPointer<AirDiff_noise> airDiff);
    // 添加 Aircondition_noise 部件
    void addAirCondition(QSharedPointer<Aircondition_noise> airCondition);
    // 添加 Circular_damper_noi 部件
    void addCircularDamper(QSharedPointer<Circular_damper_noi> circularDamper);
    // 添加 Disp_vent_terminal_noise 部件
    void addDispVentTerminal(QSharedPointer<Disp_vent_terminal_noise> dispVentTerminal);


private:
    // 禁止通过构造函数和复制构造函数创建实例
    ComponentManager() {}
    ComponentManager(const ComponentManager&) = delete;
    ComponentManager& operator=(const ComponentManager&) = delete;
};

#endif // COMPONENTMANAGER_H
