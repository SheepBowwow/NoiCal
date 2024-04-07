#include "projectmanager.h"
#include "databasemanager.h"
#include <QMessageBox>

ProjectManager::ProjectManager()
{
    this->projectIDs = DatabaseManager::getInstance().loadProjectIDs();
}

/**
 * @brief 用于设置项目信息
 * @param newPrjInfo
 */
void ProjectManager::setPrjInfo(const ProjectInfo &newPrjInfo)
{
    prjInfo = newPrjInfo;
    if(!DatabaseManager::getInstance().isProjectExist(newPrjInfo.prjID))
        DatabaseManager::getInstance().addProjectInfoToDatabase(newPrjInfo);
    else
        DatabaseManager::getInstance().updateProjectInfoInDatabase(newPrjInfo);
}

/**
 * @brief 设置项目编号
 * @param prjID
 */
void ProjectManager::setPrjID(const QString &prjID)
{
    this->prjInfo.prjID = prjID;
}

/**
 * @brief 获取项目编号
 * @return
 */
QString ProjectManager::getPrjID() const
{
    return this->prjInfo.prjID;
}

/**
 * @brief 设置船级社
 * @param classSoc
 */
void ProjectManager::setClassSoc(const QString &classSoc)
{
    this->prjInfo.classSoc = classSoc;
    DatabaseManager::getInstance().updateProjectInfoInDatabase(this->prjInfo);
}

/**
 * @brief 获取船级社
 * @return 船级社名称
 */
QString ProjectManager::getClassSoc() const
{
    return this->prjInfo.classSoc;
}

/**
 * @brief 插入项目编号到容器中, 不能有重复项
 * @param prjID
 * @return 添加成功返回 true, 否则返回 false
 */
bool ProjectManager::insertProjectIDToSet(const QString &prjID)
{
    if(projectIDs.contains(prjID))
        return false;

    projectIDs.insert(prjID);
    return true;
}

/**
 * @brief 插入项目附件到容器中, 并且不能有路径相同的文件
 * @param 项目附件结构体
 * @return 添加成功返回 true, 否则返回 false
 */
bool ProjectManager::insertAttachmentToList(const ProjectAttachment &attachment)
{
    for (auto &existingAttachment : attachments) {
        if (existingAttachment.attachName == attachment.attachName) {
            QMessageBox::critical(nullptr, "插入错误", QString("出现重复的附件:" + attachment.attachName));
            return false;
        }
    }
    attachments.append(attachment);
    if(!DatabaseManager::getInstance().addAttachmentToDatabase(attachment, getPrjID()))
    {
        qDebug() << "添加附件到数据库失败";
        return false;
    }
    return true;
}

bool ProjectManager::removeAttachmentInList(const QString &attachmentName)
{
    for (auto &existingAttachment : attachments) {
        if (existingAttachment.attachName == attachmentName) {
            attachments.removeOne(existingAttachment);
        }
    }
    if(!DatabaseManager::getInstance().delAttachmentInDatabase(attachmentName, getPrjID()))
    {
        qDebug() << "添加附件到数据库失败";
        return false;
    }
    return true;
}

/**
 * @brief 更新容器并且同步到数据库中
 * @param drawings
 */
void ProjectManager::setDrawings(const QList<Drawing> &drawings)
{
    this->drawings = drawings;
    DatabaseManager::getInstance().addDrawingsToDatabase(drawings,getPrjID());
}

/**
 * @brief 插入噪音限值到容器中
 * @param noiseLimit
 * @return 添加成功返回 true, 否则返回 false
 */
bool ProjectManager::setNoiseLimits(const QList<NoiseLimit> &noiseLimits)
{
    this->noiseLimits = noiseLimits;
    DatabaseManager::getInstance().addNoiseLimitsToDatabase(noiseLimits, getPrjID());
}

/**
 * @brief 清空Attachments
 */
void ProjectManager::clearAttachments()
{
    this->attachments.clear();
}

/**
 * @brief 清空Drawings
 */
void ProjectManager::clearDrawings()
{
    this->drawings.clear();
}

/**
 * @brief 清空NoiseLimits
 */
void ProjectManager::clearNoiseLimits()
{
    this->noiseLimits.clear();
}

QSet<QString> ProjectManager::getProjectIDs() const
{
    return projectIDs;
}
