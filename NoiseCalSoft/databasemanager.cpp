// DatabaseManager.cpp
#include "DatabaseManager.h"
#include "database/DatabaseOperations.h"
#include <QMessageBox>

DatabaseManager::DatabaseManager(const QString& dbName) {
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dbName);
    if (!db.open()) {
        qDebug() << "无法打开数据库：" << db.lastError().text();
    }

    registerAddFunctions();
}

QMap<QString, AddToDatabaseFunc> DatabaseManager::getComponentAddFuncMap() const
{
    return componentAddFuncMap;
}

DatabaseManager::~DatabaseManager() {
    db.close();
}


QSet<QString> DatabaseManager::loadProjectIDs() {
    QSet<QString> projectIDs;
    if (db.isOpen()) {
        QSqlQuery query(db);
        if (query.exec("SELECT projectID FROM project_basicInfo")) {
            while (query.next()) {
                projectIDs.insert(query.value(0).toString());
            }
        } else {
            qDebug() << "查询失败：" << query.lastError();
        }
    }
    return projectIDs;
}

void DatabaseManager::registerAddFunctions()
{
    componentAddFuncMap[component_type_name::FAN] = [this](const ComponentBase& component) -> bool {
        return DBComponentAddOperations::addFanToDatabase(component, this->db);
    };

    componentAddFuncMap[component_type_name::FANCOIL] = [this](const ComponentBase& component) -> bool {
        return DBComponentAddOperations::addFanCoilToDatabase(component, this->db);
    };

    componentAddFuncMap[component_type_name::AIRCONDITION] = [this](const ComponentBase& component) -> bool {
      return DBComponentAddOperations::addAirConditionToDatabase(component, this->db);
    };

    componentAddFuncMap[component_type_name::VAV_TERMINAL] = [this](const ComponentBase& component) -> bool {
      return DBComponentAddOperations::addVAVTerminalToDatabase(component, this->db);
    };

    componentAddFuncMap[component_type_name::CIRCULAR_DAMPER] = [this](const ComponentBase& component) -> bool {
      return DBComponentAddOperations::addCircularDamperToDatabase(component, this->db);
    };

    componentAddFuncMap[component_type_name::RECT_DAMPER] = [this](const ComponentBase& component) -> bool {
      return DBComponentAddOperations::addRectDamperToDatabase(component, this->db);
    };

    componentAddFuncMap[component_type_name::AIRDIFF] = [this](const ComponentBase& component) -> bool {
      return DBComponentAddOperations::addAirDiffToDatabase(component, this->db);
    };

    componentAddFuncMap[component_type_name:: PUMPSEND] = [this](const ComponentBase& component) -> bool {
      return DBComponentAddOperations::addPumpSendToDatabase(component, this->db);
    };

    componentAddFuncMap[component_type_name::STATICBOX_GRILLE] = [this](const ComponentBase& component) -> bool {
      return DBComponentAddOperations::addStaticBoxGrilleToDatabase(component, this->db);
    };

    componentAddFuncMap[component_type_name::DISP_VENT_TERMINAL] = [this](const ComponentBase& component) -> bool {
      return DBComponentAddOperations::addDispVentTerminalToDatabase(component, this->db);
    };

    componentAddFuncMap[component_type_name::OTHER_SEND_TERMINAL] = [this](const ComponentBase& component) -> bool {
      return DBComponentAddOperations::addOtherSendTerminalToDatabase(component, this->db);
    };

    componentAddFuncMap[component_type_name::STATICBOX] = [this](const ComponentBase& component) -> bool {
      return DBComponentAddOperations::addStaticBoxToDatabase(component, this->db);
    };

    componentAddFuncMap[component_type_name::MULTI_RANC] = [this](const ComponentBase& component) -> bool {
      return DBComponentAddOperations::addMultiRancToDatabase(component, this->db);
    };

    componentAddFuncMap[component_type_name::TEE] = [this](const ComponentBase& component) -> bool {
      return DBComponentAddOperations::addTeeToDatabase(component, this->db);
    };

    componentAddFuncMap[component_type_name::PIPE] = [this](const ComponentBase& component) -> bool {
      return DBComponentAddOperations::addPipeToDatabase(component, this->db);
    };

    componentAddFuncMap[component_type_name::ELBOW] = [this](const ComponentBase& component) -> bool {
      return DBComponentAddOperations::addPipeToDatabase(component, this->db);
    };

    componentAddFuncMap[component_type_name::REDUCER] = [this](const ComponentBase& component) -> bool {
      return DBComponentAddOperations::addPipeToDatabase(component, this->db);
    };

    componentAddFuncMap[component_type_name::SILENCER] = [this](const ComponentBase& component) -> bool {
      return DBComponentAddOperations::addPipeToDatabase(component, this->db);
    };

}

void DatabaseManager::addComponentToDel(const QString &UUID)
{
    componentToDel.append(UUID);
}

bool DatabaseManager::addProjectInfoToDatabase(const ProjectInfo &projectInfo)
{
    if (!db.isOpen()) {
        qDebug() << "数据库未打开";
        return false;
    }

    QSqlQuery query;
    query.prepare("INSERT INTO project_basicInfo (projectID, project_name, ship_num, "
                  "shipyard, project_manager, class_soc) "
                  "VALUES (:projectID, :project_name, :ship_num, :shipyard, "
                  ":project_manager, :class_soc)");

    // 绑定值
    query.bindValue(":projectID", projectInfo.prjID);
    query.bindValue(":project_name", projectInfo.prjName);
    query.bindValue(":ship_num", projectInfo.shipNum);
    query.bindValue(":shipyard", projectInfo.shipyard);
    query.bindValue(":project_manager", projectInfo.prjManager);
    query.bindValue(":class_soc", projectInfo.classSoc);

    if (!query.exec()) {
        qDebug() << "插入项目基本信息失败：" << query.lastError().text();
        return false;
    }

    if(isProjectExist(projectInfo.prjID))
        qDebug() << "插入成功";
    return true;
}

bool DatabaseManager::updateProjectInfoInDatabase(const ProjectInfo &projectInfo)
{
    // 首先检查项目ID是否存在
    if (!isProjectExist(projectInfo.prjID)) {
        qDebug() << "Project ID" + projectInfo.prjID + "does not exist, cannot update.";
        return false;
    }

    // 获取数据库连接
    QSqlDatabase& db = DatabaseManager::getInstance().getDB();

    // 检查数据库是否打开
    if (!db.isOpen()) {
        qDebug() << "Database is not open.";
        return false;
    }

    QSqlQuery query(db);
    // 准备更新语句，更新项目信息但不包括prjID
    query.prepare("UPDATE project_basicInfo SET project_name = :prjName, ship_num = :shipNum, shipyard = :shipyard, project_manager = :prjManager, class_soc = :classSoc WHERE projectID = :prjID");

    // 绑定需要更新的值
    query.bindValue(":prjName", projectInfo.prjName);
    query.bindValue(":shipNum", projectInfo.shipNum);
    query.bindValue(":shipyard", projectInfo.shipyard);
    query.bindValue(":prjManager", projectInfo.prjManager);
    query.bindValue(":classSoc", projectInfo.classSoc);
    query.bindValue(":prjID", projectInfo.prjID);

    // 执行查询
    if (!query.exec()) {
        qDebug() << "Failed to update project information: " << query.lastError();
        return false;
    }

    return true;
}

bool DatabaseManager::updateProjectIDInDatabase(const QString &old_prjID, const QString &new_prjID)
{
    // 检查新的项目ID是否已经存在
    if (isProjectExist(new_prjID)) {
        qDebug() << "Project ID" << new_prjID << "already exists. Cannot update to this ID.";
        return false;
    }

    QSqlDatabase& db = getDB();

    // 检查数据库是否打开
    if (!db.isOpen()) {
        qDebug() << "Database is not open.";
        return false;
    }

    QSqlQuery query(db);
    // 开始一个事务
    db.transaction();

    // 准备更新语句
    query.prepare("UPDATE project_basicInfo SET prjID = :new_prjID WHERE prjID = :old_prjID");
    query.bindValue(":new_prjID", new_prjID);
    query.bindValue(":old_prjID", old_prjID);

    // 尝试执行更新
    if (!query.exec()) {
        qDebug() << "Failed to update project ID from" << old_prjID << "to" << new_prjID << ":" << query.lastError().text();
        db.rollback(); // 如果失败，回滚事务
        return false;
    }

    // 提交事务
    if (!db.commit()) {
        qDebug() << "Transaction commit failed:" << db.lastError().text();
        db.rollback(); // 如果提交失败，回滚事务
        return false;
    }

    return true;
}

/**
 * @brief 添加一系列图纸记录到数据库
 *
 * 该函数首先删除指定项目ID对应的所有图纸记录，然后将新的图纸记录列表插入到数据库中。
 * 操作是在一个数据库事务中执行的，以确保数据的一致性。如果在删除旧记录或插入新记录过程中发生错误，
 * 会回滚事务以保持数据库状态不变。整个操作成功完成后，事务被提交。
 *
 * @param drawings 包含多个图纸信息的列表，每个图纸信息包含表格序号、图号、图名。
 * @param projectID 当前操作关联的项目ID，用于定位需要删除和添加记录的项目。
 *
 * @note 如果操作中途发生错误，函数将输出错误信息并提前退出。这包括无法删除旧记录、
 *       无法插入新记录以及无法提交事务的情况。
 *
 * @note 在调用此函数之前，确保传入的projectID有效，并且drawings列表中的每个项都已正确填写。
 */
void DatabaseManager::addDrawingsToDatabase(const QList<Drawing> &drawings, const QString &projectID)
{
    // 开始一个事务
    db.transaction();

    // 首先，检查projectID在project_basicInfo表中是否存在
    QSqlQuery checkQuery(getDB());
    checkQuery.prepare("SELECT COUNT(*) FROM project_basicInfo WHERE projectID = :projectID");
    checkQuery.bindValue(":projectID", projectID);
    if (!checkQuery.exec()) {
        qDebug() << "Failed to check projectID existence:" << checkQuery.lastError().text();
        db.rollback(); // 回滚事务
        return; // 提前退出
    }
    checkQuery.next();
    if (checkQuery.value(0).toInt() == 0) {
        qDebug() << "projectID" << projectID << "does not exist in project_basicInfo.";
        QMessageBox::critical(nullptr,"错误","请先完善并保存\n左侧\"工程信息\"后操作");
        db.rollback(); // 回滚事务
        return; // 提前退出
    }

    // 项目ID存在，继续执行删除与项目编号相同的所有项
    QSqlQuery query(getDB());
    query.prepare("DELETE FROM project_drawing WHERE projectID = :projectID");
    query.bindValue(":projectID", projectID);
    if (!query.exec()) {
        qDebug() << "Failed to delete existing drawings for projectID" << projectID << ":" << query.lastError().text();
        db.rollback(); // 回滚事务
        return; // 提前退出
    }

    // 然后，插入新的drawing项
    for (const Drawing &drawing : drawings) {
        query.prepare("INSERT INTO project_drawing (table_id, drawing_num, drawing_name, projectID) VALUES (:table_id, :drawing_num, :drawing_name, :projectID)");
        query.bindValue(":table_id", drawing.tableID);
        query.bindValue(":drawing_num", drawing.drawingNum);
        query.bindValue(":drawing_name", drawing.drawingName);
        query.bindValue(":projectID", projectID);

        if (!query.exec()) {
            qDebug() << "Failed to insert drawing into database:" << query.lastError().text();
            db.rollback(); // 如果有错误，回滚事务
            return; // 提前退出
        }
    }

    // 提交事务
    if (!db.commit()) {
        qDebug() << "Transaction commit failed:" << db.lastError().text();
        db.rollback();
    }
}

void DatabaseManager::addNoiseLimitsToDatabase(const QList<NoiseLimit> &noiseLimits, const QString &projectID)
{
    if (!db.isOpen()) {
        // 尝试打开数据库，或者至少记录一个错误
        qDebug() << "数据库未打开";
        return;
    }

    // 开始一个事务
    db.transaction();

    if(!isProjectExist(projectID))
    {
        QMessageBox::critical(nullptr, "错误", "请先完善并保存\n\"工程信息\"后操作");
        db.rollback(); // 回滚事务
        return; // 提前退出
    }

    // 项目ID存在，继续执行删除与项目编号相同的所有噪声限制项
    QSqlQuery query(getDB());
    query.prepare("DELETE FROM project_noiseLimit WHERE projectID = :projectID");
    query.bindValue(":projectID", projectID);
    if (!query.exec()) {
        qDebug() << "Failed to delete existing noise limits for projectID" << projectID << ":" << query.lastError().text();
        db.rollback(); // 回滚事务
        return; // 提前退出
    }

    // 然后，插入新的噪声限制项
    for (const NoiseLimit &limit : noiseLimits) {
        query.prepare("INSERT INTO project_noiseLimit (table_id, room_type, noise_limit, premises_type, projectID) VALUES (:table_id, :room_type, :noise_limit, :premises_type, :projectID)");
        query.bindValue(":table_id", limit.tableID);
        query.bindValue(":room_type", limit.roomType);
        query.bindValue(":noise_limit", limit.noiseLimit);
        query.bindValue(":premises_type", limit.premissType);
        query.bindValue(":projectID", projectID);

        if (!query.exec()) {
            qDebug() << "Failed to insert noise limit into database:" << query.lastError().text();
            db.rollback(); // 如果有错误，回滚事务
            return; // 提前退出
        }
    }

    // 提交事务
    if (!db.commit()) {
        qDebug() << "Transaction commit failed:" << db.lastError().text();
        db.rollback();
    }
}

bool DatabaseManager::addAttachmentToDatabase(const ProjectAttachment &attachment, const QString &projectID)
{
    if (!db.isOpen()) {
        // 尝试打开数据库，或者至少记录一个错误
        qDebug() << "数据库未打开";
        return false;
    }

    if(!isProjectExist(projectID))
    {
        QMessageBox::critical(nullptr, "错误", "请先完善并保存\n\"工程信息\"后操作");
        return false;
    }

    // 准备一个插入SQL命令
    QSqlQuery query;
    query.prepare("INSERT INTO project_attachmentInfo (table_id, attachment_name, attachment_path, projectID) "
                  "VALUES (:tableID, :attachName, :attachPath, :projectID)");

    // 绑定结构体中的值到SQL命令参数
    query.bindValue(":tableID", attachment.tableID);
    query.bindValue(":attachName", attachment.attachName);
    query.bindValue(":attachPath", attachment.attachPath);
    query.bindValue(":projectID", projectID);

    // 执行SQL命令
    if (!query.exec()) {
        // 如果执行失败，记录错误
        qDebug() << "添加附件到数据库失败：" << query.lastError().text();
        return false;
    }

    qDebug() << "成功添加附件信息到数据库";
}

bool DatabaseManager::delAttachmentInDatabase(const QString &attachmentName, const QString &projectID)
{
    if (!db.isOpen()) {
            qDebug() << "数据库未打开";
            return false;
        }

        // 准备一个删除SQL命令
        QSqlQuery query;
        query.prepare("DELETE FROM project_attachmentInfo WHERE attachment_name = :attachName AND projectID = :projectID");

        // 绑定给定的附件名和项目ID到SQL命令的参数
        query.bindValue(":attachName", attachmentName);
        query.bindValue(":projectID", projectID);

        // 执行SQL命令
        if (!query.exec()) {
            // 如果执行失败，记录错误并返回false
            qDebug() << "删除附件失败：" << query.lastError().text();
            return false;
        }

        // 检查是否真的有行被删除了（可选）
        if (query.numRowsAffected() <= 0) {
            qDebug() << "未找到匹配的附件进行删除";
            return false;
        }

        // 如果一切顺利，返回true
        return true;
}

bool DatabaseManager::isProjectExist(const QString &prjID)
{
    // 获取数据库连接
    QSqlDatabase& db = getDB();

    // 检查数据库是否打开
    if (!db.isOpen()) {
        qDebug() << "Database is not open.";
        return false;
    }

    QSqlQuery query(db);
    // 准备SQL查询语句，用于检查指定的prjID是否存在
    query.prepare("SELECT 1 FROM project_basicInfo WHERE projectID = :prjID");
    // 绑定参数
    query.bindValue(":prjID", prjID);

    // 执行查询
    if (query.exec()) {
        // 查询成功，检查是否有结果
        if (query.next()) {
            // 如果有结果，说明找到了对应的prjID，返回true
            return true;
        }
    } else {
        // 查询失败，打印错误信息
        qDebug() << "Query failed: " << query.lastError();
    }

    // 默认返回false，表示没有找到对应的prjID
    return false;
}
