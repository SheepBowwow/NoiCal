// DatabaseManager.cpp
#include "database/DatabaseOperations.h"
#include "databasemanager.h"
#include <QMessageBox>
#include <QFile>
#include <QDir>
#include <QStandardPaths>
#include <QSqlRecord>
#include "Component/ComponentManager.h"

QHash<QString, QString> typeNameToTableName = {
    {component_type_name::FAN, "fan"},
    {component_type_name::FANCOIL, "fancoil"},
    {component_type_name::AIRCONDITION, "aircondition"},
    {component_type_name::VAV_TERMINAL, "vav_terminal"},
    {component_type_name::CIRCULAR_DAMPER, "circular_damper"},
    {component_type_name::RECT_DAMPER, "rect_damper"},
    {component_type_name::AIRDIFF, "air_diff"},
    {component_type_name::PUMPSEND, "pump_send"},
    {component_type_name::STATICBOX_GRILLE, "static_box_grille"},
    {component_type_name::DISP_VENT_TERMINAL, "disp_vent_terminal"},
    {component_type_name::OTHER_SEND_TERMINAL, "other_send_terminal"},
    {component_type_name::STATICBOX, "static_box"},
    {component_type_name::MULTI_RANC, "multi_ranc"},
    {component_type_name::TEE, "tee"},
    {component_type_name::PIPE, "pipe"},
    {component_type_name::ELBOW, "elbow"},
    {component_type_name::REDUCER, "reducer"},
    {component_type_name::SILENCER, "silencer"}
};

DatabaseManager::DatabaseManager(const QString& dbName) {
    // 获取应用数据的路径
    QString dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    // 创建项目名文件夹的路径
    QString projectDirPath = dataDir;

    // 使用QDir创建文件夹
    QDir dir;
    if (!dir.exists(projectDirPath)) {
        dir.mkpath(projectDirPath);
    }

    // 在项目文件夹内构建数据库文件的完整路径
    QString dbPath = projectDirPath + "/noi_cal_database.db";  // 修改了文件名以符合你的需求

    // 检查数据库文件是否已经存在
    if (!QFile::exists(dbPath)) {
        // 从资源中复制数据库文件到目标位置，注意资源文件的路径也进行了相应的调整
        QFile::copy(":/databaseFile/databaseFile/noi_cal_database_origin.db", dbPath); // 修改了资源路径和文件名
        // 确保目标数据库文件是可写的
        QFile::setPermissions(dbPath, QFile::ReadOwner | QFile::WriteOwner);
    }
    // 连接到数据库
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dbPath);
    if (!db.open()) {
        qDebug() << "Error opening database:" << db.lastError();
        return;
    }
    else
    {
        // 数据库成功打开后，启用外键支持
        QSqlQuery query;
        if (!query.exec("PRAGMA foreign_keys=ON;")) {
            qDebug() << "无法启用外键支持：" << query.lastError().text();
        }
    }


    registerAddFunctions();
    registerUpdateFunctions();
}

QMap<QString, AddToDatabaseFunc> DatabaseManager::getComponentUpdateFuncMap() const
{
    return componentUpdateFuncMap;
}

QMap<QString, AddToDatabaseFunc> DatabaseManager::getComponentAddFuncMap() const
{
    return componentAddFuncMap;
}

DatabaseManager::~DatabaseManager() {
    db.close();
}

/**
 * @brief 程序开始的时候加载所有的项目编号到容器里，方便comboBox调用
 * @return
 */
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

ProjectInfo DatabaseManager::getProjectInfoFromDB(const QString &projectID) const
{
    if (!db.isOpen()) {
        // 处理连接失败
        return ProjectInfo(); // 返回默认构造的空结构体
    }

    // 准备查询
    QSqlQuery query;
    query.prepare("SELECT projectID, project_name, ship_num, shipyard, project_manager, class_soc "
                  "FROM project_basicInfo "
                  "WHERE projectID = :projectID");

    // 绑定参数
    query.bindValue(":projectID", projectID);

    // 执行查询
    if (!query.exec()) {
        // 处理查询失败
        qDebug() << "Query failed:" << query.lastError().text();
        return ProjectInfo(); // 返回默认构造的空结构体
    }

    // 获取查询结果
    if (query.next()) {
        QSqlRecord record = query.record();

        // 从记录中提取数据
        QString prjID = record.value("projectID").toString();
        QString prjName = record.value("project_name").toString();
        QString shipNum = record.value("ship_num").toString();
        QString shipyard = record.value("shipyard").toString();
        QString prjManager = record.value("project_manager").toString();
        QString classSoc = record.value("class_soc").toString();

        // 创建并返回 ProjectInfo 结构体
        return ProjectInfo(prjID, prjName, shipNum, shipyard, prjManager, classSoc);
    } else {
        // 未找到匹配的项目
        return ProjectInfo(); // 返回默认构造的空结构体
    }
}

bool DatabaseManager::getProjectAttachments(const QString &projectID, QList<ProjectAttachment> &attachments)
{
    attachments.clear(); // 确保开始时列表是空的

    if (!db.isOpen()) {
        // 可以在这里打印日志或设置错误信息
        return false;
    }

    QSqlQuery query(db);
    query.prepare("SELECT table_id, attachment_name, attachment_path FROM project_attachmentInfo WHERE projectID = :projectID ORDER BY table_id ASC");
    query.bindValue(":projectID", projectID);

    if (!query.exec()) {
        // 可以在这里打印日志或设置错误信息
        return false;
    }

    while (query.next()) {
        QSqlRecord record = query.record();
        ProjectAttachment attachment{
            record.value("table_id").toString(),
                    record.value("attachment_name").toString(),
                    record.value("attachment_path").toString()};
        attachments.append(attachment);
    }

    // 如果执行到这里，说明查询成功执行，即使没有数据返回也视为成功
    return true;
}

bool DatabaseManager::getProjectDrawings(const QString &projectID, QList<Drawing> &drawings)
{
    drawings.clear(); // 确保开始时列表是空的

    if (!db.isOpen()) {
        // 可以在这里打印日志或设置错误信息
        return false;
    }

    QSqlQuery query(db);
    query.prepare("SELECT table_id, drawing_num, drawing_name FROM project_drawing "
                  "WHERE projectID = :projectID ORDER BY table_id ASC");
    query.bindValue(":projectID", projectID);

    if (!query.exec()) {
        // 可以在这里打印日志或设置错误信息
        return false;
    }

    while (query.next()) {
        QSqlRecord record = query.record();
        Drawing drawing{
            record.value("table_id").toString(),
                    record.value("drawing_num").toString(),
                    record.value("drawing_name").toString()};
        drawings.append(drawing);
    }

    // 如果执行到这里，说明查询成功执行，即使没有数据返回也视为成功
    return true;
}

bool DatabaseManager::getProjectNoiseLimit(const QString &projectID, QList<NoiseLimit> &noiseLimits)
{
    noiseLimits.clear(); // 确保开始时列表是空的

    if (!db.isOpen()) {
        // 可以在这里打印日志或设置错误信息
        return false;
    }

    QSqlQuery query(db);
    query.prepare("SELECT table_id, room_type, noise_limit, premises_type "
                  "FROM project_noiseLimit WHERE projectID = :projectID ORDER BY table_id ASC");
    query.bindValue(":projectID", projectID);

    if (!query.exec()) {
        // 可以在这里打印日志或设置错误信息
        return false;
    }

    while (query.next()) {
        QSqlRecord record = query.record();
        NoiseLimit noiseLimit{
            record.value("table_id").toString(),
                    record.value("room_type").toString(),
                    record.value("noise_limit").toString(), // 注意：根据实际数据类型，可能需要调整
                    record.value("premises_type").toString()};
        noiseLimits.append(noiseLimit);
    }

    // 如果执行到这里，说明查询成功执行，即使没有数据返回也视为成功
    return true;
}

/**
 * @brief 注册各种组件的添加到数据库的函数到函数映射表中。
 *
 * 此方法通过组件类型为键，将特定的添加组件到数据库的函数绑定到函数映射表（componentAddFuncMap）中。
 * 每个绑定的函数都负责处理将相应类型的组件数据安全有效地添加到数据库中。
 * 通过这种方式，当需要添加一个新的组件时，可以通过查询该映射表来动态调用对应的添加函数，
 * 实现了添加逻辑的解耦和动态扩展。
 *
 * 这种注册机制简化了对不同组件处理函数的管理，使得为新的组件类型添加支持变得更加灵活和高效。
 * 如果需要支持新的组件类型，只需实现对应的添加到数据库的函数，并在此方法中进行注册即可。
 *
 * 注意：在调用任何添加组件的函数之前，确保数据库连接已正确设置并打开。
 */
void DatabaseManager::registerAddFunctions()
{
    componentAddFuncMap[component_type_name::FAN] = [this](const ComponentBase& component) -> bool {
        return DBComponentAddOperations::addOrUpdateFanToDatabase(component, this->db);
    };

    componentAddFuncMap[component_type_name::FANCOIL] = [this](const ComponentBase& component) -> bool {
        return DBComponentAddOperations::addOrUpdateFanCoilToDatabase(component, this->db);
    };

    componentAddFuncMap[component_type_name::AIRCONDITION] = [this](const ComponentBase& component) -> bool {
        return DBComponentAddOperations::addOrUpdateAirConditionToDatabase(component, this->db);
    };

    componentAddFuncMap[component_type_name::VAV_TERMINAL] = [this](const ComponentBase& component) -> bool {
        return DBComponentAddOperations::addOrUpdateVAVTerminalToDatabase(component, this->db);
    };

    componentAddFuncMap[component_type_name::CIRCULAR_DAMPER] = [this](const ComponentBase& component) -> bool {
        return DBComponentAddOperations::addOrUpdateCircularDamperToDatabase(component, this->db);
    };

    componentAddFuncMap[component_type_name::RECT_DAMPER] = [this](const ComponentBase& component) -> bool {
        return DBComponentAddOperations::addOrUpdateRectDamperToDatabase(component, this->db);
    };

    componentAddFuncMap[component_type_name::AIRDIFF] = [this](const ComponentBase& component) -> bool {
        return DBComponentAddOperations::addOrUpdateAirDiffToDatabase(component, this->db);
    };

    componentAddFuncMap[component_type_name:: PUMPSEND] = [this](const ComponentBase& component) -> bool {
        return DBComponentAddOperations::addOrUpdatePumpSendToDatabase(component, this->db);
    };

    componentAddFuncMap[component_type_name::STATICBOX_GRILLE] = [this](const ComponentBase& component) -> bool {
        return DBComponentAddOperations::addOrUpdateStaticBoxGrilleToDatabase(component, this->db);
    };

    componentAddFuncMap[component_type_name::DISP_VENT_TERMINAL] = [this](const ComponentBase& component) -> bool {
        return DBComponentAddOperations::addOrUpdateDispVentTerminalToDatabase(component, this->db);
    };

    componentAddFuncMap[component_type_name::OTHER_SEND_TERMINAL] = [this](const ComponentBase& component) -> bool {
        return DBComponentAddOperations::addOrUpdateOtherSendTerminalToDatabase(component, this->db);
    };

    componentAddFuncMap[component_type_name::STATICBOX] = [this](const ComponentBase& component) -> bool {
        return DBComponentAddOperations::addOrUpdateStaticBoxToDatabase(component, this->db);
    };

    componentAddFuncMap[component_type_name::MULTI_RANC] = [this](const ComponentBase& component) -> bool {
        return DBComponentAddOperations::addOrUpdateMultiRancToDatabase(component, this->db);
    };

    componentAddFuncMap[component_type_name::TEE] = [this](const ComponentBase& component) -> bool {
        return DBComponentAddOperations::addOrUpdateTeeToDatabase(component, this->db);
    };

    componentAddFuncMap[component_type_name::PIPE] = [this](const ComponentBase& component) -> bool {
        return DBComponentAddOperations::addOrUpdatePipeToDatabase(component, this->db);
    };

    componentAddFuncMap[component_type_name::ELBOW] = [this](const ComponentBase& component) -> bool {
        return DBComponentAddOperations::addOrUpdateElbowToDatabase(component, this->db);
    };

    componentAddFuncMap[component_type_name::REDUCER] = [this](const ComponentBase& component) -> bool {
        return DBComponentAddOperations::addOrUpdateReducerToDatabase(component, this->db);
    };

    componentAddFuncMap[component_type_name::SILENCER] = [this](const ComponentBase& component) -> bool {
        return DBComponentAddOperations::addOrUpdateSilencerToDatabase(component, this->db);
    };

}

void DatabaseManager::registerUpdateFunctions()
{
    componentUpdateFuncMap[component_type_name::FAN] = [this](const ComponentBase& component) -> bool {
        return DBComponentAddOperations::addOrUpdateFanToDatabase(component, this->db, true);
    };

    componentUpdateFuncMap[component_type_name::FANCOIL] = [this](const ComponentBase& component) -> bool {
        return DBComponentAddOperations::addOrUpdateFanCoilToDatabase(component, this->db, true);
    };

    componentUpdateFuncMap[component_type_name::AIRCONDITION] = [this](const ComponentBase& component) -> bool {
        return DBComponentAddOperations::addOrUpdateAirConditionToDatabase(component, this->db, true);
    };

    componentUpdateFuncMap[component_type_name::VAV_TERMINAL] = [this](const ComponentBase& component) -> bool {
        return DBComponentAddOperations::addOrUpdateVAVTerminalToDatabase(component, this->db, true);
    };

    componentUpdateFuncMap[component_type_name::CIRCULAR_DAMPER] = [this](const ComponentBase& component) -> bool {
        return DBComponentAddOperations::addOrUpdateCircularDamperToDatabase(component, this->db, true);
    };

    componentUpdateFuncMap[component_type_name::RECT_DAMPER] = [this](const ComponentBase& component) -> bool {
        return DBComponentAddOperations::addOrUpdateRectDamperToDatabase(component, this->db, true);
    };

    componentUpdateFuncMap[component_type_name::AIRDIFF] = [this](const ComponentBase& component) -> bool {
        return DBComponentAddOperations::addOrUpdateAirDiffToDatabase(component, this->db, true);
    };

    componentUpdateFuncMap[component_type_name:: PUMPSEND] = [this](const ComponentBase& component) -> bool {
        return DBComponentAddOperations::addOrUpdatePumpSendToDatabase(component, this->db, true);
    };

    componentUpdateFuncMap[component_type_name::STATICBOX_GRILLE] = [this](const ComponentBase& component) -> bool {
        return DBComponentAddOperations::addOrUpdateStaticBoxGrilleToDatabase(component, this->db, true);
    };

    componentUpdateFuncMap[component_type_name::DISP_VENT_TERMINAL] = [this](const ComponentBase& component) -> bool {
        return DBComponentAddOperations::addOrUpdateDispVentTerminalToDatabase(component, this->db, true);
    };

    componentUpdateFuncMap[component_type_name::OTHER_SEND_TERMINAL] = [this](const ComponentBase& component) -> bool {
        return DBComponentAddOperations::addOrUpdateOtherSendTerminalToDatabase(component, this->db, true);
    };

    componentUpdateFuncMap[component_type_name::STATICBOX] = [this](const ComponentBase& component) -> bool {
        return DBComponentAddOperations::addOrUpdateStaticBoxToDatabase(component, this->db, true);
    };

    componentUpdateFuncMap[component_type_name::MULTI_RANC] = [this](const ComponentBase& component) -> bool {
        return DBComponentAddOperations::addOrUpdateMultiRancToDatabase(component, this->db, true);
    };

    componentUpdateFuncMap[component_type_name::TEE] = [this](const ComponentBase& component) -> bool {
        return DBComponentAddOperations::addOrUpdateTeeToDatabase(component, this->db, true);
    };

    componentUpdateFuncMap[component_type_name::PIPE] = [this](const ComponentBase& component) -> bool {
        return DBComponentAddOperations::addOrUpdatePipeToDatabase(component, this->db, true);
    };

    componentUpdateFuncMap[component_type_name::ELBOW] = [this](const ComponentBase& component) -> bool {
        return DBComponentAddOperations::addOrUpdateElbowToDatabase(component, this->db, true);
    };

    componentUpdateFuncMap[component_type_name::REDUCER] = [this](const ComponentBase& component) -> bool {
        return DBComponentAddOperations::addOrUpdateReducerToDatabase(component, this->db, true);
    };

    componentUpdateFuncMap[component_type_name::SILENCER] = [this](const ComponentBase& component) -> bool {
        return DBComponentAddOperations::addOrUpdateSilencerToDatabase(component, this->db, true);
    };

}


/**
 * @brief 根据不同的表去删除对应uuid的部件
 * @param componentName
 * @param UUID
 */
void DatabaseManager::delComponentInDatabase(const QString& componentName ,const QString &UUID)
{
    DBComponentDelOperations::deleteComponentFromDatabase(typeNameToTableName[componentName], UUID, this->db);
}

bool DatabaseManager::delProjectInDatabase(const QString &prjID)
{
    // 首先，我们需要确保给定的 projectID 不为空
    if (prjID.trimmed().isEmpty()) {
        qDebug() << "Project ID is empty, cannot delete project from database.";
        return false;
    }

    // 准备 SQL DELETE 语句
    QSqlQuery query;
    query.prepare("DELETE FROM project_basicInfo WHERE projectID = :projectID");

    // 绑定要删除的项目的 projectID
    query.bindValue(":projectID", prjID);

    // 执行查询
    if (!query.exec()) {
        qDebug() << "Failed to delete project from database:" << query.lastError().text();
        return false;
    }

    // 检查是否有行被成功删除
    if (query.numRowsAffected() <= 0) {
        qDebug() << "No rows affected. Possibly the project ID does not exist.";
        return false;
    }

    return true;
}

bool DatabaseManager::addProjectInfoToDatabase(const ProjectInfo &projectInfo, bool initProject)
{
    if (!db.isOpen()) {
        qDebug() << "数据库未打开";
        return false;
    }

    QSqlQuery query;
    // 无条件准备完整查询
    query.prepare("INSERT INTO project_basicInfo (projectID, project_name, ship_num, shipyard, project_manager, class_soc) "
                  "VALUES (:projectID, :project_name, :ship_num, :shipyard, :project_manager, :class_soc)");

    if (initProject) {
        // 仅绑定projectID进行部分插入
        query.bindValue(":projectID", projectInfo.prjID);
    } else {
        // 绑定所有变量进行完整插入
        query.bindValue(":projectID", projectInfo.prjID);
        query.bindValue(":project_name", projectInfo.prjName);
        query.bindValue(":ship_num", projectInfo.shipNum);
        query.bindValue(":shipyard", projectInfo.shipyard);
        query.bindValue(":project_manager", projectInfo.prjManager);
        query.bindValue(":class_soc", projectInfo.classSoc);
    }

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
    query.prepare("UPDATE project_basicInfo SET projectID = :new_prjID WHERE projectID = :old_prjID");
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

void DatabaseManager::loadComponentsFromDatabase()
{
    // 遍历所有组件类型的名称
    for (auto it = typeNameToTableName.constBegin(); it != typeNameToTableName.constEnd(); ++it) {
        const QString& componentTypeName = it.key();
        const QString& tableName = it.value();

        QSqlQuery query;
        query.prepare("SELECT * FROM " + tableName + " WHERE projectID = :projectID");
        query.bindValue(":projectID", ProjectManager::getInstance().getPrjID());

        if (!query.exec()) {
            qDebug() << "Query failed for table" << tableName << ":" << query.lastError().text();
            continue;
        }

        while (query.next()) {
            QSqlRecord record = query.record();

            // 动态创建组件对象
            QSharedPointer<ComponentBase> component;

            if (componentTypeName == component_type_name::FAN) {
                component = QSharedPointer<ComponentBase>(new Fan(record));
            } else if (componentTypeName == component_type_name::FANCOIL) {
                component = QSharedPointer<ComponentBase>(new FanCoil(record));
            } else if (componentTypeName == component_type_name::AIRCONDITION) {
                component = QSharedPointer<ComponentBase>(new Aircondition(record));
            } else if (componentTypeName == component_type_name::VAV_TERMINAL) {
                component = QSharedPointer<ComponentBase>(new VAV_terminal(record));
            } else if (componentTypeName == component_type_name::CIRCULAR_DAMPER) {
                component = QSharedPointer<ComponentBase>(new Circular_damper(record));
            } else if (componentTypeName == component_type_name::RECT_DAMPER) {
                component = QSharedPointer<ComponentBase>(new Rect_damper(record));
            } else if (componentTypeName == component_type_name::AIRDIFF) {
                component = QSharedPointer<ComponentBase>(new AirDiff(record));
            } else if (componentTypeName == component_type_name::PUMPSEND) {
                component = QSharedPointer<ComponentBase>(new PumpSend(record));
            } else if (componentTypeName == component_type_name::STATICBOX_GRILLE) {
                component = QSharedPointer<ComponentBase>(new StaticBox_grille(record));
            } else if (componentTypeName == component_type_name::DISP_VENT_TERMINAL) {
                component = QSharedPointer<ComponentBase>(new Disp_vent_terminal(record));
            } else if (componentTypeName == component_type_name::OTHER_SEND_TERMINAL) {
                component = QSharedPointer<ComponentBase>(new Other_send_terminal(record));
            } else if (componentTypeName == component_type_name::STATICBOX) {
                component = QSharedPointer<ComponentBase>(new Static_box(record));
            } else if (componentTypeName == component_type_name::MULTI_RANC) {
                component = QSharedPointer<ComponentBase>(new Multi_ranc(record));
            } else if (componentTypeName == component_type_name::TEE) {
                component = QSharedPointer<ComponentBase>(new Tee(record));
            } else if (componentTypeName == component_type_name::PIPE) {
                component = QSharedPointer<ComponentBase>(new Pipe(record));
            } else if (componentTypeName == component_type_name::ELBOW) {
                component = QSharedPointer<ComponentBase>(new Elbow(record));
            } else if (componentTypeName == component_type_name::REDUCER) {
                component = QSharedPointer<ComponentBase>(new Reducer(record));
            } else if (componentTypeName == component_type_name::SILENCER) {
                component = QSharedPointer<ComponentBase>(new Silencer(record));
            } else {
                qDebug() << "Unknown component type name:" << componentTypeName;
                continue;
            }

            // 将组件添加到管理器中
            if (component) {
                ComponentManager::getInstance().addComponent(component, true);
            } else {
                qDebug() << "Failed to create component for table" << tableName;
            }
        }
    }
}
