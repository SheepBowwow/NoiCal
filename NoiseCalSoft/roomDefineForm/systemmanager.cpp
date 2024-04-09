#include "systemmanager.h"

SystemManager::SystemManager()
{

}

/**
 * @brief SystemManager::addComponentToSystemMap
 * @param systemName    系统名
 * @param componentType 部件类型
 * @param coponentUUID  UUID
 */
void SystemManager::addComponentToSystemMap(const QString& systemName, const QString& componentType,
                                            const QString& coponentUUID)
{
    systemSrcComponentsMap[systemName][componentType].append(coponentUUID);
}

/**
 * @brief SystemManager::getNoiseComponentsInSystem
 * @param systemName
 * @param componentType
 * @return 返回某一种类部件的 uuid list
 */
QList<QString> SystemManager::getNoiseComponentsInSystem(const QString &systemName, const QString &componentType)
{
    QList<QString> componentUUIDs = systemSrcComponentsMap[systemName][componentType];
    return componentUUIDs;
}
