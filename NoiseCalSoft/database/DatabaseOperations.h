#ifndef DATABASEOPERATIONS_H
#define DATABASEOPERATIONS_H

#include <QSqlDatabase>
#include "Component/ComponentStructs.h"

/**
 * 用于addmap映射
 */
namespace DBComponentAddOperations {

bool addOrUpdateFanToDatabase(const ComponentBase& component, QSqlDatabase& db, bool update = false);
bool addOrUpdateFanCoilToDatabase(const ComponentBase& component, QSqlDatabase& db, bool update = false);
bool addOrUpdateAirConditionToDatabase(const ComponentBase& component, QSqlDatabase& db, bool update = false);
bool addOrUpdateVAVTerminalToDatabase(const ComponentBase& component, QSqlDatabase& db, bool update = false);
bool addOrUpdateCircularDamperToDatabase(const ComponentBase& component, QSqlDatabase& db, bool update = false);
bool addOrUpdateRectDamperToDatabase(const ComponentBase& component, QSqlDatabase& db, bool update = false);
bool addOrUpdateAirDiffToDatabase(const ComponentBase& component, QSqlDatabase& db, bool update = false);
bool addOrUpdatePumpSendToDatabase(const ComponentBase& component, QSqlDatabase& db, bool update = false);
bool addOrUpdateStaticBoxGrilleToDatabase(const ComponentBase& component, QSqlDatabase& db, bool update = false);
bool addOrUpdateDispVentTerminalToDatabase(const ComponentBase& component, QSqlDatabase& db, bool update = false);
bool addOrUpdateOtherSendTerminalToDatabase(const ComponentBase& component, QSqlDatabase& db, bool update = false);
bool addOrUpdateStaticBoxToDatabase(const ComponentBase& component, QSqlDatabase& db, bool update = false);
bool addOrUpdateMultiRancToDatabase(const ComponentBase& component, QSqlDatabase& db, bool update = false);
bool addOrUpdateDiffuserBranchToDatabase(const ComponentBase& component, QSqlDatabase& db, bool update = false);
bool addOrUpdateTeeToDatabase(const ComponentBase& component, QSqlDatabase& db, bool update = false);
bool addOrUpdatePipeToDatabase(const ComponentBase& component, QSqlDatabase& db, bool update = false);
bool addOrUpdateElbowToDatabase(const ComponentBase& component, QSqlDatabase& db, bool update = false);
bool addOrUpdateReducerToDatabase(const ComponentBase& component, QSqlDatabase& db, bool update = false);
bool addOrUpdateSilencerToDatabase(const ComponentBase& component, QSqlDatabase& db, bool update = false);

}

namespace DBComponentDelOperations {

/**
 * @brief 从数据库中删除部件
 * @param 数据库表名
 * @param UUID
 * @param db
 * @return
 */
bool deleteComponentFromDatabase(const QString &tableName, const QString &UUID, QSqlDatabase &db);

}

#endif // DATABASEOPERATIONS_H
