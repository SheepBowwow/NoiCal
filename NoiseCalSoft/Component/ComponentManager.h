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
#include <QTableWidget>
#include <QObject>

class ComponentManager : public QObject
{
    Q_OBJECT
public:
    static ComponentManager& getInstance() {
        static ComponentManager instance; // 在首次调用时创建唯一实例
        return instance;
    }

    // 添加组件
    void addComponent(const QSharedPointer<ComponentBase>& component, bool updateOrLoad = false);
    // 删除组件
    bool removeComponent(const QString& uuid, bool update = false);
    // 查找组件
    QSharedPointer<ComponentBase> findComponent(const QString& uuid) const;

    // 获取特定类型的所有组件
    QList<QSharedPointer<ComponentBase>> getComponentsByType(const QString& type) const;

    // 修改组件
    bool updateComponent(const QString& uuid, const QSharedPointer<ComponentBase>& newComponent);

    void clearCurrentPrjComponents();

    void loadComponentToHash();

signals:
    void loadComponentsDone();
    void componentsUpdate(const QString &uuid);

private:
    // 禁止通过构造函数和复制构造函数创建实例
    ComponentManager() {};
    ComponentManager(const ComponentManager&) = delete;
    ComponentManager& operator=(const ComponentManager&) = delete;

    QHash<QString, QSharedPointer<ComponentBase>> components;   //用来存放所有部件的hash, key: UUID, value: Component
    QHash<QString, QList<QSharedPointer<ComponentBase>>> componentsByType;  //不同种类的容器
};

#endif // COMPONENTMANAGER_H
