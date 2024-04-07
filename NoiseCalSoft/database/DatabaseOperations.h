#ifndef DATABASEOPERATIONS_H
#define DATABASEOPERATIONS_H

#include <QSqlDatabase>
#include "Component/ComponentStructs.h"

namespace DBComponentAddOperations {

bool addFanToDatabase(const ComponentBase& component, QSqlDatabase& db);
bool addFanCoilToDatabase(const ComponentBase& component, QSqlDatabase& db);
bool addAirConditionToDatabase(const ComponentBase& component, QSqlDatabase& db);
bool addVAVTerminalToDatabase(const ComponentBase& component, QSqlDatabase& db);
bool addCircularDamperToDatabase(const ComponentBase& component, QSqlDatabase& db);
bool addRectDamperToDatabase(const ComponentBase& component, QSqlDatabase& db);
bool addAirDiffToDatabase(const ComponentBase& component, QSqlDatabase& db);
bool addPumpSendToDatabase(const ComponentBase& component, QSqlDatabase& db);
bool addStaticBoxGrilleToDatabase(const ComponentBase& component, QSqlDatabase& db);
bool addDispVentTerminalToDatabase(const ComponentBase& component, QSqlDatabase& db);
bool addOtherSendTerminalToDatabase(const ComponentBase& component, QSqlDatabase& db);
bool addStaticBoxToDatabase(const ComponentBase& component, QSqlDatabase& db);
bool addMultiRancToDatabase(const ComponentBase& component, QSqlDatabase& db);
bool addTeeToDatabase(const ComponentBase& component, QSqlDatabase& db);
bool addPipeToDatabase(const ComponentBase& component, QSqlDatabase& db);
bool addElbowToDatabase(const ComponentBase& component, QSqlDatabase& db);
bool addReducerToDatabase(const ComponentBase& component, QSqlDatabase& db);
bool addSilencerToDatabase(const ComponentBase& component, QSqlDatabase& db);



}

#endif // DATABASEOPERATIONS_H
