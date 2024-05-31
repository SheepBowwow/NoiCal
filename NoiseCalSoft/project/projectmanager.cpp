#include "projectmanager.h"
#include "databasemanager.h"
#include <QMessageBox>

ProjectManager::ProjectManager()
{
    this->projectIDs = DatabaseManager::getInstance().loadProjectIDs();
}

bool ProjectManager::isProjectOpened()
{
    //如果项目id为空就是没有打开项目
    return !this->getPrjID().trimmed().isEmpty();
}

ProjectInfo ProjectManager::getPrjInfo() const
{
    return prjInfo;
}

QList<NoiseLimit> ProjectManager::getNoiseLimits() const
{
    return noiseLimits;
}

QList<Drawing> ProjectManager::getDrawings() const
{
    return drawings;
}

QList<ProjectAttachment> ProjectManager::getAttachments() const
{
    return attachments;
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
void ProjectManager::setPrjID(const QString &prjID, bool initProject)
{
    this->prjInfo.prjID = prjID;
    if(initProject)
        DatabaseManager::getInstance().addProjectInfoToDatabase(this->prjInfo, true);
}

void ProjectManager::delPrj(const QString &prjID)
{
    DatabaseManager::getInstance().delProjectInDatabase(prjID);    //删除数据库中的
    this->projectIDs.remove(prjID);
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
 * @brief 清空项目信息
 */
void ProjectManager::clearCurrentPrjData()
{
    this->prjInfo = ProjectInfo{};
    this->attachments.clear();
    this->drawings.clear();
    this->noiseLimits.clear();

    emit clearTable();
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
    return true; // 假设添加操作成功，返回true
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

/**
 * @brief 从数据库中获取数据保存在容器中
 * @return
 */
bool ProjectManager::loadProjectInfo()
{
    prjInfo = DatabaseManager::getInstance().getProjectInfoFromDB(prjInfo.prjID);
    if (prjInfo.isValid()) {
        return true; // 数据有效
    } else {
        return false; // 数据无效或查询失败
    }
}

bool ProjectManager::loadAttachments()
{
    if(!DatabaseManager::getInstance().getProjectAttachments(prjInfo.prjID, this->attachments))
    {
        qDebug() << "load attachments error";
        return false;
    }
    return true;
}

bool ProjectManager::loadDrawings()
{
    if(!DatabaseManager::getInstance().getProjectDrawings(prjInfo.prjID, this->drawings))
    {
        qDebug() << "load drawings error";
        return false;
    }
    return true;
}

bool ProjectManager::loadNoiseLimits()
{
    if(!DatabaseManager::getInstance().getProjectNoiseLimit(prjInfo.prjID, this->noiseLimits))
    {
        qDebug() << "load noiseLimits error";
        return false;
    }
    return true;
}

bool ProjectManager::switchProjectToDo()
{
    // 加载项目信息
    if (!loadProjectInfo()) {
        qDebug() << "Error loading project info";
        return false;
    }

    // 加载附件
    if (!loadAttachments()) {
        qDebug() << "Error loading attachments";
        return false;
    }

    // 加载图纸
    if (!loadDrawings()) {
        qDebug() << "Error loading drawings";
        return false;
    }

    // 加载噪声限制信息
    if (!loadNoiseLimits()) {
        qDebug() << "Error loading noise limits";
        return false;
    }

    // 如果所有步骤都成功执行，则返回true
    emit loadBasicInfoDone();   //发送更新成功信号，widget接受并更新表格
    return true;
}

QSet<QString> ProjectManager::getProjectIDs() const
{
    return projectIDs;
}
