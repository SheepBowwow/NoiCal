#include "ComponentManager.h"
#include <QtGlobal>
#include <QSharedPointer>
#include "databasemanager.h"

/**
 * @brief 添加部件到容器，如果是更新操作就不与数据库交互
 * @param component
 * @param update
 */
void ComponentManager::addComponent(const QSharedPointer<ComponentBase> &component, bool inComponentDB, bool updateOrLoad)
{
    if(!inComponentDB)
    {
        components[component->UUID] = component;
        componentsByType[component->typeName()].append(component);
    }
    else
    {
        DBcomponents[component->UUID] = component;
        DBcomponentsByType[component->typeName()].append(component);
    }

    if(!updateOrLoad)
    {
        // 查找并尝试执行添加到数据库的函数
        const auto& addFuncMap = DatabaseManager::getInstance().getComponentAddFuncMap();
        // 然后在这个引用上调用 find()
        auto addFunc = addFuncMap.find(component->typeName());
        qDebug() << addFunc.key();
        // 检查是否找到并尝试执行添加到数据库的函数
        if (addFunc != addFuncMap.end() && addFunc.value()(*component, inComponentDB)) {
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
bool ComponentManager::removeComponent(const QString &uuid, bool inComponentDB, bool update)
{
    QString typeName;
    QHash<QString, QSharedPointer<ComponentBase>>* container;
    QHash<QString, QList<QSharedPointer<ComponentBase>>>* containerBytype;
    if(!inComponentDB)
    {
        container = &components;
        containerBytype = &componentsByType;
    }
    else
    {
        container = &DBcomponents;
        containerBytype = &DBcomponentsByType;
    }

    if (!container->contains(uuid)) return false;
    auto component = (*container)[uuid];
    container->remove(uuid);
    if(!(*containerBytype)[component->typeName()].removeOne(component))
    {
        qDebug() << "Failed to remove component from list";
    }
    typeName = component->typeName();


    if(!update)
        DatabaseManager::getInstance().delComponentInDatabase(typeName, uuid, inComponentDB);
    emit componentsUpdate(uuid);
    return true;
}

QSharedPointer<ComponentBase> ComponentManager::findComponent(bool inComponentDB, const QString &uuid) const
{
    if(!inComponentDB)
        return components.value(uuid, QSharedPointer<ComponentBase>(nullptr));
    else
        return DBcomponents.value(uuid, QSharedPointer<ComponentBase>(nullptr));
}

QList<QSharedPointer<ComponentBase> > ComponentManager::getComponentsByType(bool inComponentDB, const QString &type) const
{
    QList<QSharedPointer<ComponentBase> > res;
    if(inComponentDB)
        res = DBcomponentsByType.value(type, QList<QSharedPointer<ComponentBase>>());
    else
        res = componentsByType.value(type, QList<QSharedPointer<ComponentBase>>());
    return res;
}

bool ComponentManager::updateComponent(const QString &uuid, const QSharedPointer<ComponentBase> &newComponent, bool inComponentDB)
{
    if(!inComponentDB)
    {
        if (!components.contains(uuid)) return false;
    }
    else
    {
        if (!DBcomponents.contains(uuid)) return false;
    }
    // 首先删除旧组件
    removeComponent(uuid, inComponentDB ,true);
    // 添加新组件
    addComponent(newComponent, inComponentDB, true);
    // 查找并尝试执行添加到数据库的函数
    const auto& updateFuncMap = DatabaseManager::getInstance().getComponentUpdateFuncMap();
    // 然后在这个引用上调用 find()
    auto updateFunc = updateFuncMap.find(newComponent->typeName());
    // 检查是否找到并尝试执行添加到数据库的函数
    if (updateFunc != updateFuncMap.end() && updateFunc.value()(*newComponent, inComponentDB)) {
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
    DatabaseManager::getInstance().loadComponentsFromPrjDB();

    emit loadComponentsDone();
}
