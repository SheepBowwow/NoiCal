#include "ComponentManager.h"
#include <QtGlobal>
#include <QSharedPointer>


void ComponentManager::addComponent(const QSharedPointer<ComponentBase> &component)
{
    components[component->UUID] = component;
    componentsByType[component->typeName()].append(component);
}

bool ComponentManager::removeComponent(const QString &uuid)
{
    if (!components.contains(uuid)) return false;
    auto component = components[uuid];
    components.remove(uuid);
    componentsByType[component->typeName()].removeOne(component);
    return true;
}

QSharedPointer<ComponentBase> ComponentManager::findComponent(const QString &uuid) const
{
    return components.value(uuid, QSharedPointer<ComponentBase>(nullptr));
}

QList<QSharedPointer<ComponentBase> > ComponentManager::getComponentsByType(const QString &type) const
{
    return componentsByType.value(type, QList<QSharedPointer<ComponentBase>>());
}

bool ComponentManager::updateComponent(const QString &uuid, const QSharedPointer<ComponentBase> &newComponent)
{
    if (!components.contains(uuid)) return false;
    // 首先删除旧组件
    removeComponent(uuid);
    // 添加新组件
    addComponent(newComponent);
    return true;
}
