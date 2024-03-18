#ifndef COMPONENTMANAGER_H
#define COMPONENTMANAGER_H

#include <unordered_map>
#include <QString>
#include <QList>
#include <QSharedPointer>
#include <QtGlobal> // 确保包含了 QtGlobal 头文件
#include <QDebug>
#include "Component/ComponentStructs.h"
#include <string>
#include <regex>

class ComponentManager
{
public:
    static ComponentManager& getInstance() {
        static ComponentManager instance; // 在首次调用时创建唯一实例
        return instance;
    }

    void addComponent(QSharedPointer<ComponentBase> component, QString type);
    void updateRevisedComponent(QString preName ,QSharedPointer<ComponentBase> component, QString type);

    void del_and_updateTableID(int deleteID, QString containerName);

    QVector<QString> getAirconditionNumbers();
    QVector<QString> getFanNumbers();
    QVector<QString> getFanCoilNumbers();

    QString getModelByNumber(QString number);
    ComponentBase* getComponentByNumber(QString number);
    ComponentBase* getComponentByModel(QString model, QString type);
    QList<const ComponentBase*> getComponents(QString type);

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

    template<typename T>
    void del_and_updateTableIDBase(QList<QSharedPointer<T>> list, int deleteID, QString containerName);
};

#endif // COMPONENTMANAGER_H
