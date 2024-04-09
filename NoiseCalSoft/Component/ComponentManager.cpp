#include "ComponentManager.h"
#include <QtGlobal>
#include <QSharedPointer>
#include "databasemanager.h"

/**
 * @brief 添加部件到容器，如果是更新操作就不与数据库交互
 * @param component
 * @param update
 */
void ComponentManager::addComponent(const QSharedPointer<ComponentBase> &component, bool updateOrLoad)
{
    components[component->UUID] = component;
    componentsByType[component->typeName()].append(component);

    if(!updateOrLoad)
    {
        // 查找并尝试执行添加到数据库的函数
        const auto& addFuncMap = DatabaseManager::getInstance().getComponentAddFuncMap();
        // 然后在这个引用上调用 find()
        auto addFunc = addFuncMap.find(component->typeName());
        qDebug() << addFunc.key();
        // 检查是否找到并尝试执行添加到数据库的函数
        if (addFunc != addFuncMap.end() && addFunc.value()(*component)) {
            qDebug() << component->typeName() << "added to database successfully.";
        } else {
            qDebug() << "Failed to add" << component->typeName() << "to database.";
        }
    }

    emit componentsUpdate(component->UUID);
}

/**
 * @brief 移除容器中的部件，如果是更新部件就补删除数据库中的部件了
 * @param uuid
 * @param update 是否为更新部件调用
 * @return
 */
bool ComponentManager::removeComponent(const QString &uuid, bool update)
{
    if (!components.contains(uuid)) return false;
    auto component = components[uuid];
    components.remove(uuid);
    componentsByType[component->typeName()].removeOne(component);

    if(!update)
        DatabaseManager::getInstance().delComponentInDatabase(component->typeName(), uuid);
    emit componentsUpdate(uuid);
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
    removeComponent(uuid, true);
    // 添加新组件
    addComponent(newComponent, true);
    // 查找并尝试执行添加到数据库的函数
    const auto& updateFuncMap = DatabaseManager::getInstance().getComponentUpdateFuncMap();
    // 然后在这个引用上调用 find()
    auto updateFunc = updateFuncMap.find(newComponent->typeName());
    // 检查是否找到并尝试执行添加到数据库的函数
    if (updateFunc != updateFuncMap.end() && updateFunc.value()(*newComponent)) {
        qDebug() << newComponent->typeName() << "update to database successfully.";
    } else {
        qDebug() << "Failed to update" << newComponent->typeName() << "to database.";
    }
    emit componentsUpdate(uuid);
    return true;
}

void ComponentManager::clearCurrentPrjComponents()
{
    this->components.clear();
    this->componentsByType.clear();
}

void ComponentManager::loadComponentToHash()
{
    DatabaseManager::getInstance().loadComponentsFromDatabase();

    emit loadComponentsDone();
}
