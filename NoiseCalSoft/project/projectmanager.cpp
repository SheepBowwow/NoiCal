#include "projectmanager.h"
#include "databasemanager.h"

ProjectManager::ProjectManager()
{
    this->projectIDs = DatabaseManager::instance().loadProjectIDs();
}

void ProjectManager::setPrjInfo(const ProjectInfo &newPrjInfo)
{
    prjInfo = newPrjInfo;
}

void ProjectManager::setPrjID(const QString &prjID)
{
    this->prjInfo.prjID = prjID;
}

QSet<QString> ProjectManager::getProjectIDs() const
{
    return projectIDs;
}
