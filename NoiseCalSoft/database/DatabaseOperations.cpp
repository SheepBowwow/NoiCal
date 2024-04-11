#include "DatabaseOperations.h"
#include "project/projectmanager.h"
#include <QSqlQuery>
#include <QDebug>
#include <QSqlError>
#include <QJsonDocument>
#include <QJsonArray>

namespace DBComponentAddOperations {



bool addOrUpdateFanToDatabase(const ComponentBase& component, QSqlDatabase& db, bool update)
{
    if (!db.isOpen()) {
        // 确保数据库已经打开
        qWarning() << "Database is not open!";
        return false;
    }

    // 动态类型检查以确保传入的ComponentBase实际上是一个Fan
    const Fan* fan = dynamic_cast<const Fan*>(&component);
    if (!fan) {
        qWarning() << "Component is not a Fan";
        return false;
    }

    // 将噪音数组转换成 JSON 字符串
    QJsonArray noiInArray;
    for (const auto& noi : fan->noi_in) {
        noiInArray.append(noi);
    }
    QJsonDocument docIn(noiInArray);
    QString noiInJsonString = QString::fromUtf8(docIn.toJson());

    QJsonArray noiOutArray;
    for (const auto& noi : fan->noi_out) {
        noiOutArray.append(noi);
    }
    QJsonDocument docOut(noiOutArray);
    QString noiOutJsonString = QString::fromUtf8(docOut.toJson());

    // 创建并准备 SQL 插入语句
    QSqlQuery query(db);
    if (update) {
        // 准备更新语句
        query.prepare("UPDATE fan SET projectID=:projectID, table_id=:table_id, number=:number, model=:model, "
                      "air_volume=:air_volume, static_pressure=:static_pressure, brand=:brand, noise_in_json=:noise_in_json, "
                      "noise_out_json=:noise_out_json, data_source=:data_source WHERE UUID=:UUID");
    } else {
        // 准备插入语句
        query.prepare("INSERT INTO fan (projectID, table_id, number, model, air_volume, "
                      "static_pressure, brand, noise_in_json, noise_out_json, data_source, UUID) "
                      "VALUES (:projectID, :table_id, :number, :model, :air_volume, "
                      ":static_pressure, :brand, :noise_in_json, :noise_out_json, :data_source, :UUID)");
    }
    // 绑定值到插入语句
    query.bindValue(":projectID", ProjectManager::getInstance().getPrjID());
    query.bindValue(":table_id", fan->table_id);
    query.bindValue(":number", fan->number);
    query.bindValue(":model", fan->model);
    query.bindValue(":air_volume", fan->air_volume.toDouble());
    query.bindValue(":static_pressure", fan->static_pressure.toDouble());
    query.bindValue(":brand", fan->brand);
    query.bindValue(":noise_in_json", noiInJsonString);
    query.bindValue(":noise_out_json", noiOutJsonString);
    query.bindValue(":data_source", fan->data_source);
    query.bindValue(":UUID", fan->UUID);

    // 执行 SQL 插入操作
    if (!query.exec()) {
        qWarning() << "Operation failed:" << query.lastError();
        return false;
    }

    return true;
}

bool addOrUpdateFanCoilToDatabase(const ComponentBase& component, QSqlDatabase& db, bool update)
{
    if (!db.isOpen()) {
        qWarning() << "Database is not open!";
        return false;
    }

    // 使用dynamic_cast尝试将ComponentBase转换为FanCoil类型
    const FanCoil* fanCoil = dynamic_cast<const FanCoil*>(&component);
    if (!fanCoil) {
        qWarning() << "Component is not a FanCoil";
        return false;
    }

    // 将噪音数组转换为JSON字符串
    QJsonArray noiInArray;
    for (const auto& noi : fanCoil->noi_in) {
        noiInArray.append(noi);
    }
    QJsonDocument docIn(noiInArray);
    QString noiInJsonString = QString::fromUtf8(docIn.toJson());

    QJsonArray noiOutArray;
    for (const auto& noi : fanCoil->noi_out) {
        noiOutArray.append(noi);
    }
    QJsonDocument docOut(noiOutArray);
    QString noiOutJsonString = QString::fromUtf8(docOut.toJson());

    // 创建并准备SQL插入语句
    QSqlQuery query(db);
    if (update) {
        // 准备更新语句
        query.prepare("UPDATE fanCoil "
                      "SET projectID=:projectID, table_id=:table_id, type=:type, model=:model, air_volume=:air_volume, "
                      "static_pressure=:static_pressure, brand=:brand, noise_in_json=:noise_in_json, "
                      "noise_out_json=:noise_out_json, data_source=:data_source "
                      "WHERE UUID=:UUID");
    } else {
        // 准备插入语句
        query.prepare("INSERT INTO fanCoil (projectID, table_id, type, model, air_volume, "
                      "static_pressure, brand, noise_in_json, noise_out_json, data_source, UUID) "
                      "VALUES (:projectID, :table_id, :type, :model, :air_volume, :static_pressure, "
                      ":brand, :noise_in_json, :noise_out_json, :data_source, :UUID)");
    }


    // 绑定值到插入语句
    query.bindValue(":projectID", ProjectManager::getInstance().getPrjID());
    query.bindValue(":table_id", fanCoil->table_id.toInt());
    query.bindValue(":type", fanCoil->type);
    query.bindValue(":model", fanCoil->model);
    query.bindValue(":air_volume", fanCoil->air_volume.toDouble());
    query.bindValue(":static_pressure", fanCoil->static_pressure.toDouble());
    query.bindValue(":brand", fanCoil->brand);
    query.bindValue(":noise_in_json", noiInJsonString);
    query.bindValue(":noise_out_json", noiOutJsonString);
    query.bindValue(":data_source", fanCoil->data_source);
    query.bindValue(":UUID", fanCoil->UUID);

    // 执行SQL插入操作
    if (!query.exec()) {
        qWarning() << "Operation failed:" << query.lastError();
        return false;
    }

    return true;
}

bool addOrUpdateAirConditionToDatabase(const ComponentBase& component, QSqlDatabase& db, bool update)
{
    if (!db.isOpen()) {
        qWarning() << "Database is not open!";
        return false;
    }

    // 使用dynamic_cast尝试将ComponentBase转换为Aircondition类型
    const Aircondition* aircondition = dynamic_cast<const Aircondition*>(&component);
    if (!aircondition) {
        qWarning() << "Component is not an Aircondition";
        return false;
    }

    // 噪音数据的转换成JSON
    auto toJson = [](const std::array<QString, 9>& noises) -> QString {
        QJsonArray jsonArray;
        for (const auto& noise : noises) {
            jsonArray.append(noise);
        }
        QJsonDocument doc(jsonArray);
        return QString::fromUtf8(doc.toJson());
    };

    // 准备SQL插入语句
    QSqlQuery query(db);
    if (update) {
        query.prepare("UPDATE aircondition "
                      "SET projectID=:projectID, table_id=:table_id, fan_counts=:fan_counts, send_number=:send_number, "
                      "send_air_volume=:send_air_volume, send_static_pressure=:send_static_pressure, exhaust_number=:exhaust_number, "
                      "exhaust_air_volume=:exhaust_air_volume, exhaust_static_pressure=:exhaust_static_pressure, model=:model, "
                      "brand=:brand, noise_send_in_json=:noise_send_in_json, noise_send_out_json=:noise_send_out_json, "
                      "noise_exhaust_in_json=:noise_exhaust_in_json, noise_exhaust_out_json=:noise_exhaust_out_json, "
                      "data_source=:data_source "
                      "WHERE UUID=:UUID");
    } else {
        query.prepare("INSERT INTO aircondition (projectID, table_id, fan_counts, send_number, "
                      "send_air_volume, send_static_pressure, exhaust_number, exhaust_air_volume, "
                      "exhaust_static_pressure, model, brand, noise_send_in_json, noise_send_out_json, "
                      "noise_exhaust_in_json, noise_exhaust_out_json, data_source, UUID) "
                      "VALUES (:projectID, :table_id, :fan_counts, :send_number, :send_air_volume, "
                      ":send_static_pressure, :exhaust_number, :exhaust_air_volume, :exhaust_static_pressure, "
                      ":model, :brand, :noise_send_in_json, :noise_send_out_json, :noise_exhaust_in_json, "
                      ":noise_exhaust_out_json, :data_source, :UUID)");
    }
    // 绑定值
    query.bindValue(":projectID", ProjectManager::getInstance().getPrjID());
    query.bindValue(":table_id", aircondition->table_id.toInt());
    query.bindValue(":fan_counts", aircondition->fan_counts);
    query.bindValue(":send_number", aircondition->send_number);
    query.bindValue(":send_air_volume", aircondition->send_air_volume.toDouble());
    query.bindValue(":send_static_pressure", aircondition->send_static_pressure.toDouble());
    query.bindValue(":exhaust_number", aircondition->exhaust_number);
    query.bindValue(":exhaust_air_volume", aircondition->exhaust_air_volume.toDouble());
    query.bindValue(":exhaust_static_pressure", aircondition->exhaust_static_pressure.toDouble());
    query.bindValue(":model", aircondition->model);
    query.bindValue(":brand", aircondition->brand);
    query.bindValue(":noise_send_in_json", toJson(aircondition->noi_send_in));
    query.bindValue(":noise_send_out_json", toJson(aircondition->noi_send_out));
    query.bindValue(":noise_exhaust_in_json", toJson(aircondition->noi_exhaust_in));
    query.bindValue(":noise_exhaust_out_json", toJson(aircondition->noi_exhaust_out));
    query.bindValue(":data_source", aircondition->data_source);
    query.bindValue(":UUID", aircondition->UUID);

    // 执行插入
    if (!query.exec()) {
        qWarning() << "Insert operation failed:" << query.lastError();
        return false;
    }

    return true;
}

bool addOrUpdateVAVTerminalToDatabase(const ComponentBase& component, QSqlDatabase& db, bool update)
{
    if (!db.isOpen()) {
        qWarning() << "Database is not open!";
        return false;
    }

    // 使用dynamic_cast尝试将ComponentBase转换为VAV_terminal类型
    const VAV_terminal* vavTerminal = dynamic_cast<const VAV_terminal*>(&component);
    if (!vavTerminal) {
        qWarning() << "Component is not a VAV_terminal";
        return false;
    }

    // 噪音数据的转换成JSON
    QJsonArray jsonArray;
    for (const auto& noise : vavTerminal->noi) {
        jsonArray.append(noise);
    }
    QJsonDocument doc(jsonArray);
    QString noiJsonString = QString::fromUtf8(doc.toJson());

    // 准备SQL插入语句
    QSqlQuery query(db);
    if (update) {
        query.prepare("UPDATE vav_terminal "
                      "SET projectID=:projectID, table_id=:table_id, number=:number, model=:model, "
                      "valve_angle=:valve_angle, air_volume=:air_volume, brand=:brand, noise_json=:noise_json, "
                      "data_source=:data_source "
                      "WHERE UUID=:UUID");
    } else {
        query.prepare("INSERT INTO vav_terminal (projectID, table_id, number, model, "
                      "valve_angle, air_volume, brand, noise_json, data_source, UUID) "
                      "VALUES (:projectID, :table_id, :number, :model, :valve_angle, "
                      ":air_volume, :brand, :noise_json, :data_source, :UUID)");
    }

    // 绑定值
    query.bindValue(":projectID", ProjectManager::getInstance().getPrjID()); // 假设ProjectManager::getInstance().getPrjID()是已经在某处定义好的当前项目ID
    query.bindValue(":table_id", vavTerminal->table_id.toInt());
    query.bindValue(":number", vavTerminal->number);
    query.bindValue(":model", vavTerminal->model);
    query.bindValue(":valve_angle", vavTerminal->angle);
    query.bindValue(":air_volume", vavTerminal->air_volume);
    query.bindValue(":brand", vavTerminal->brand);
    query.bindValue(":noise_json", noiJsonString);
    query.bindValue(":data_source", vavTerminal->data_source);
    query.bindValue(":UUID", vavTerminal->UUID);

    // 执行插入
    if (!query.exec()) {
        qWarning() << "Insert operation failed:" << query.lastError();
        return false;
    }

    return true;
}

bool addOrUpdateCircularDamperToDatabase(const ComponentBase& component, QSqlDatabase& db, bool update)
{
    if (!db.isOpen()) {
        qWarning() << "Database is not open!";
        return false;
    }

    // Attempt to cast ComponentBase to Circular_damper
    const Circular_damper* damper = dynamic_cast<const Circular_damper*>(&component);
    if (!damper) {
        qWarning() << "Component is not a Circular_damper";
        return false;
    }

    // Convert noise data to JSON
    QJsonArray jsonArray;
    for (const auto& noise : damper->noi) {
        jsonArray.append(noise);
    }
    QJsonDocument noiseDoc(jsonArray);
    QString noiseJsonString = QString::fromUtf8(noiseDoc.toJson());

    // Prepare SQL insert statement
    QSqlQuery query(db);
    if (update) {
        query.prepare("UPDATE circular_damper "
                      "SET projectID=:projectID, table_id=:table_id, size=:size, model=:model, "
                      "valve_angle=:valve_angle, air_volume=:air_volume, brand=:brand, noise_json=:noise_json, "
                      "data_source=:data_source "
                      "WHERE UUID=:UUID");
    } else {
        query.prepare("INSERT INTO circular_damper (projectID, table_id, size, "
                      "model, valve_angle, air_volume, brand, noise_json, data_source, UUID) "
                      "VALUES (:projectID, :table_id, :size, :model, :valve_angle, "
                      ":air_volume, :brand, :noise_json, :data_source, :UUID)");
    }

    // Bind values to the insert statement
    query.bindValue(":projectID", ProjectManager::getInstance().getPrjID());
    query.bindValue(":table_id", damper->table_id.toInt());
    query.bindValue(":size", damper->diameter); // Using 'diameter' for 'size'
    query.bindValue(":model", damper->model);
    query.bindValue(":valve_angle", damper->angle);
    query.bindValue(":air_volume", damper->air_volume);
    query.bindValue(":brand", damper->brand);
    query.bindValue(":noise_json", noiseJsonString);
    query.bindValue(":data_source", damper->data_source); // Assuming data_source is applicable as is
    query.bindValue(":UUID", damper->UUID);

    // Execute the insert operation
    if (!query.exec()) {
        qWarning() << "Insert operation failed:" << query.lastError();
        return false;
    }

    return true;
}

bool addOrUpdateRectDamperToDatabase(const ComponentBase& component, QSqlDatabase& db, bool update)
{
    if (!db.isOpen()) {
        qWarning() << "Database is not open!";
        return false;
    }

    // Attempt to cast ComponentBase to Rect_damper
    const Rect_damper* damper = dynamic_cast<const Rect_damper*>(&component);
    if (!damper) {
        qWarning() << "Component is not a Rect_damper";
        return false;
    }

    // Convert noise data to JSON
    QJsonArray jsonArray;
    for (const auto& noise : damper->noi) {
        jsonArray.append(noise);
    }
    QJsonDocument noiseDoc(jsonArray);
    QString noiseJsonString = QString::fromUtf8(noiseDoc.toJson());

    // Prepare SQL insert statement
    QSqlQuery query(db);
    if (update) {
        query.prepare("UPDATE rect_damper "
                      "SET projectID=:projectID, table_id=:table_id, model=:model, brand=:brand, "
                      "size=:size, valve_angle=:valve_angle, air_volume=:air_volume, noise_json=:noise_json, "
                      "data_source=:data_source "
                      "WHERE UUID=:UUID");
    } else {
        query.prepare("INSERT INTO rect_damper (projectID, table_id, model, "
                      "brand, size, valve_angle, air_volume, noise_json, data_source, UUID) "
                      "VALUES (:projectID, :table_id, :model, :brand, :size, :valve_angle, "
                      ":air_volume, :noise_json, :data_source, :UUID)");
    }

    // Bind values to the insert statement
    query.bindValue(":projectID", ProjectManager::getInstance().getPrjID());
    query.bindValue(":table_id", damper->table_id);
    query.bindValue(":model", damper->model);
    query.bindValue(":brand", damper->brand);
    query.bindValue(":size", damper->size); // Using combined length and width
    query.bindValue(":valve_angle", damper->angle);
    query.bindValue(":air_volume", damper->air_volume);
    query.bindValue(":noise_json", noiseJsonString);
    query.bindValue(":data_source", damper->data_source);
    query.bindValue(":UUID", damper->UUID);

    // Execute the insert operation
    if (!query.exec()) {
        qWarning() << "Insert operation failed:" << query.lastError();
        return false;
    }

    return true;
}

bool addOrUpdateAirDiffToDatabase(const ComponentBase& component, QSqlDatabase& db, bool update)
{
    if (!db.isOpen()) {
        qWarning() << "Database is not open!";
        return false;
    }

    // Attempt to cast ComponentBase to AirDiff
    const AirDiff* diffuser = dynamic_cast<const AirDiff*>(&component);
    if (!diffuser) {
        qWarning() << "Component is not an AirDiff";
        return false;
    }

    // Convert noise, attenuation, and reflection data to JSON
    QJsonDocument noiseDoc(QJsonArray::fromStringList(QList<QString>::fromStdList(std::list<QString>(diffuser->noi.begin(), diffuser->noi.end()))));
    QString noiseJsonString = noiseDoc.toJson();

    QJsonDocument attenDoc(QJsonArray::fromStringList(QList<QString>::fromStdList(std::list<QString>(diffuser->atten.begin(), diffuser->atten.end()))));
    QString attenJsonString = attenDoc.toJson();

    QJsonDocument reflDoc(QJsonArray::fromStringList(QList<QString>::fromStdList(std::list<QString>(diffuser->refl.begin(), diffuser->refl.end()))));
    QString reflJsonString = reflDoc.toJson();

    // Prepare SQL insert statement
    QSqlQuery query(db);
    if (update) {
        query.prepare("UPDATE air_diff "
                      "SET projectID=:projectID, table_id=:table_id, air_distributor_model=:air_distributor_model, "
                      "air_distributor_brand=:air_distributor_brand, "
                      "diffuser_model=:diffuser_model, diffuser_brand=:diffuser_brand, terminal_shape=:terminal_shape, "
                      "terminal_size=:terminal_size, "
                      "noise_json=:noise_json, atten_json=:atten_json, refl_json=:refl_json, "
                      "noi_data_source=:noi_data_source, "
                      "atten_data_source=:atten_data_source, refl_data_source=:refl_data_source "
                      "WHERE UUID=:UUID");
    } else {
        query.prepare("INSERT INTO air_diff (projectID, table_id, air_distributor_model, "
                      "air_distributor_brand, diffuser_model, diffuser_brand, terminal_shape, "
                      "terminal_size, noise_json, atten_json, refl_json, noi_data_source, "
                      "atten_data_source, refl_data_source, UUID) "
                      "VALUES (:projectID, :table_id, :air_distributor_model, :air_distributor_brand, "
                      ":diffuser_model, :diffuser_brand, :terminal_shape, :terminal_size, :noise_json, "
                      ":atten_json, :refl_json, :noi_data_source, :atten_data_source, :refl_data_source, :UUID)");
    }

    // Bind values to the insert statement
    query.bindValue(":projectID", ProjectManager::getInstance().getPrjID());
    query.bindValue(":table_id", diffuser->table_id);
    query.bindValue(":air_distributor_model", diffuser->air_distributor_model);
    query.bindValue(":air_distributor_brand", diffuser->air_distributor_brand);
    query.bindValue(":diffuser_model", diffuser->diffuser_model);
    query.bindValue(":diffuser_brand", diffuser->diffuser_brand);
    query.bindValue(":terminal_shape", diffuser->terminal_shape);
    query.bindValue(":terminal_size", diffuser->terminal_size);
    query.bindValue(":noise_json", noiseJsonString);
    query.bindValue(":atten_json", attenJsonString);
    query.bindValue(":refl_json", reflJsonString);
    query.bindValue(":noi_data_source", diffuser->noi_data_source);
    query.bindValue(":atten_data_source", diffuser->atten_data_source);
    query.bindValue(":refl_data_source", diffuser->refl_data_source);
    query.bindValue(":UUID", diffuser->UUID);

    // Execute the insert operation
    if (!query.exec()) {
        qWarning() << "Insert operation failed:" << query.lastError();
        return false;
    }

    return true;
}

bool addOrUpdatePumpSendToDatabase(const ComponentBase& component, QSqlDatabase& db, bool update)
{
    if (!db.isOpen()) {
        qWarning() << "Database is not open!";
        return false;
    }

    // Attempt to cast ComponentBase to PumpSend
    const PumpSend* pumpSend = dynamic_cast<const PumpSend*>(&component);
    if (!pumpSend) {
        qWarning() << "Component is not a PumpSend";
        return false;
    }

    // Convert noise, attenuation, and reflection data to JSON
    QJsonDocument noiseDoc(QJsonArray::fromStringList(QList<QString>::fromStdList(std::list<QString>(pumpSend->noi.begin(), pumpSend->noi.end()))));
    QString noiseJsonString = noiseDoc.toJson(QJsonDocument::Compact);

    QJsonDocument attenDoc(QJsonArray::fromStringList(QList<QString>::fromStdList(std::list<QString>(pumpSend->atten.begin(), pumpSend->atten.end()))));
    QString attenJsonString = attenDoc.toJson(QJsonDocument::Compact);

    QJsonDocument reflDoc(QJsonArray::fromStringList(QList<QString>::fromStdList(std::list<QString>(pumpSend->refl.begin(), pumpSend->refl.end()))));
    QString reflJsonString = reflDoc.toJson(QJsonDocument::Compact);

    // Prepare SQL insert statement
    QSqlQuery query(db);
    if (update) {
        query.prepare("UPDATE pump_send "
                      "SET projectID=:projectID, table_id=:table_id, type_pump_or_send=:type_pump_or_send, model=:model, "
                      "terminal_shape=:terminal_shape, terminal_size=:terminal_size, brand=:brand, noise_json=:noise_json, "
                      "atten_json=:atten_json, refl_json=:refl_json, noi_data_source=:noi_data_source, "
                      "atten_data_source=:atten_data_source, refl_data_source=:refl_data_source "
                      "WHERE UUID=:UUID");
    } else {
        query.prepare("INSERT INTO pump_send (projectID, table_id, type_pump_or_send, model, "
                      "terminal_shape, terminal_size, brand, noise_json, atten_json, refl_json, "
                      "noi_data_source, atten_data_source, refl_data_source, UUID) "
                      "VALUES (:projectID, :table_id, :type_pump_or_send, :model, :terminal_shape, "
                      ":terminal_size, :brand, :noise_json, :atten_json, :refl_json, :noi_data_source, "
                      ":atten_data_source, :refl_data_source, :UUID)");
    }

    // Bind values to the insert statement
    query.bindValue(":projectID", ProjectManager::getInstance().getPrjID());
    query.bindValue(":table_id", pumpSend->table_id.toInt());
    query.bindValue(":type_pump_or_send", pumpSend->type_pump_or_send);
    query.bindValue(":model", pumpSend->model);
    query.bindValue(":terminal_shape", pumpSend->terminal_shape);
    query.bindValue(":terminal_size", pumpSend->terminal_size);
    query.bindValue(":brand", pumpSend->brand);
    query.bindValue(":noise_json", noiseJsonString);
    query.bindValue(":atten_json", attenJsonString);
    query.bindValue(":refl_json", reflJsonString);
    query.bindValue(":noi_data_source", pumpSend->noi_data_source);
    query.bindValue(":atten_data_source", pumpSend->atten_data_source);
    query.bindValue(":refl_data_source", pumpSend->refl_data_source);
    query.bindValue(":UUID", pumpSend->UUID);

    // Execute the insert operation
    if (!query.exec()) {
        qWarning() << "Insert operation failed:" << query.lastError();
        return false;
    }

    return true;
}

bool addOrUpdateStaticBoxGrilleToDatabase(const ComponentBase& component, QSqlDatabase& db, bool update)
{
    if (!db.isOpen()) {
        qWarning() << "Database is not open!";
        return false;
    }

    // Attempt to cast ComponentBase to StaticBox_grille
    const StaticBox_grille* staticBoxGrille = dynamic_cast<const StaticBox_grille*>(&component);
    if (!staticBoxGrille) {
        qWarning() << "Component is not a StaticBox_grille";
        return false;
    }

    // Convert noise, attenuation, and reflection data to JSON
    QJsonDocument noiseDoc(QJsonArray::fromStringList(QList<QString>::fromStdList(std::list<QString>(staticBoxGrille->noi.begin(), staticBoxGrille->noi.end()))));
    QString noiseJsonString = noiseDoc.toJson(QJsonDocument::Compact);

    QJsonDocument attenDoc(QJsonArray::fromStringList(QList<QString>::fromStdList(std::list<QString>(staticBoxGrille->atten.begin(), staticBoxGrille->atten.end()))));
    QString attenJsonString = attenDoc.toJson(QJsonDocument::Compact);

    QJsonDocument reflDoc(QJsonArray::fromStringList(QList<QString>::fromStdList(std::list<QString>(staticBoxGrille->refl.begin(), staticBoxGrille->refl.end()))));
    QString reflJsonString = reflDoc.toJson(QJsonDocument::Compact);

    // Prepare SQL insert statement
    QSqlQuery query(db);
    if (update) {
        query.prepare("UPDATE static_box_grille "
                      "SET projectID=:projectID, table_id=:table_id, static_box_model=:static_box_model, "
                      "static_box_brand=:static_box_brand, "
                      "grille_model=:grille_model, grille_brand=:grille_brand, terminal_shape=:terminal_shape, "
                      "terminal_size=:terminal_size, "
                      "noise_json=:noise_json, atten_json=:atten_json, refl_json=:refl_json, noi_data_source=:noi_data_source, "
                      "atten_data_source=:atten_data_source, refl_data_source=:refl_data_source "
                      "WHERE UUID=:UUID");
    } else {
        query.prepare("INSERT INTO static_box_grille (projectID, table_id, static_box_model, static_box_brand, "
                      "grille_model, grille_brand, terminal_shape, terminal_size, noise_json, atten_json, "
                      "refl_json, noi_data_source, atten_data_source, refl_data_source, UUID) "
                      "VALUES (:projectID, :table_id, :static_box_model, :static_box_brand, :grille_model, "
                      ":grille_brand, :terminal_shape, :terminal_size, :noise_json, :atten_json, :refl_json, "
                      ":noi_data_source, :atten_data_source, :refl_data_source, :UUID)");
    }

    // Bind values to the insert statement
    query.bindValue(":projectID", ProjectManager::getInstance().getPrjID());
    query.bindValue(":table_id", staticBoxGrille->table_id.toInt());
    query.bindValue(":static_box_model", staticBoxGrille->staticBox_model);
    query.bindValue(":static_box_brand", staticBoxGrille->staticBox_brand);
    query.bindValue(":grille_model", staticBoxGrille->grille_model);
    query.bindValue(":grille_brand", staticBoxGrille->grille_brand);
    query.bindValue(":terminal_shape", staticBoxGrille->terminal_shape);
    query.bindValue(":terminal_size", staticBoxGrille->terminal_size);
    query.bindValue(":noise_json", noiseJsonString);
    query.bindValue(":atten_json", attenJsonString);
    query.bindValue(":refl_json", reflJsonString);
    query.bindValue(":noi_data_source", staticBoxGrille->noi_data_source);
    query.bindValue(":atten_data_source", staticBoxGrille->atten_data_source);
    query.bindValue(":refl_data_source", staticBoxGrille->refl_data_source);
    query.bindValue(":UUID", staticBoxGrille->UUID);

    // Execute the insert operation
    if (!query.exec()) {
        qWarning() << "Insert operation failed:" << query.lastError();
        return false;
    }

    return true;

}

bool addOrUpdateDispVentTerminalToDatabase(const ComponentBase& component, QSqlDatabase& db, bool update)
{
    if (!db.isOpen()) {
        qWarning() << "Database is not open!";
        return false;
    }

    // Attempt to cast ComponentBase to Terminal
    const Terminal* terminal = dynamic_cast<const Terminal*>(&component);
    if (!terminal) {
        qWarning() << "Component is not a Terminal";
        return false;
    }

    // Convert noise, attenuation, and reflection data to JSON
    QJsonDocument noiseDoc(QJsonArray::fromStringList(QList<QString>::fromStdList(std::list<QString>(terminal->noi.begin(), terminal->noi.end()))));
    QString noiseJsonString = noiseDoc.toJson(QJsonDocument::Compact);

    QJsonDocument attenDoc(QJsonArray::fromStringList(QList<QString>::fromStdList(std::list<QString>(terminal->atten.begin(), terminal->atten.end()))));
    QString attenJsonString = attenDoc.toJson(QJsonDocument::Compact);

    QJsonDocument reflDoc(QJsonArray::fromStringList(QList<QString>::fromStdList(std::list<QString>(terminal->refl.begin(), terminal->refl.end()))));
    QString reflJsonString = reflDoc.toJson(QJsonDocument::Compact);

    // Prepare SQL insert statement
    QSqlQuery query(db);
    if (update) {
        query.prepare("UPDATE disp_vent_terminal "
                      "SET projectID=:projectID, table_id=:table_id, model=:model, terminal_shape=:terminal_shape, "
                      "terminal_size=:terminal_size, brand=:brand, nosie_json=:nosie_json, atten_json=:atten_json, "
                      "refl_json=:refl_json, noi_data_source=:noi_data_source, atten_data_source=:atten_data_source, "
                      "refl_data_source=:refl_data_source "
                      "WHERE UUID=:UUID");
    } else {
        query.prepare("INSERT INTO disp_vent_terminal (projectID, table_id, model, terminal_shape, "
                      "terminal_size, brand, nosie_json, atten_json, refl_json, noi_data_source, "
                      "atten_data_source, refl_data_source, UUID) "
                      "VALUES (:projectID, :table_id, :model, :terminal_shape, :terminal_size, "
                      ":brand, :nosie_json, :atten_json, :refl_json, :noi_data_source, :atten_data_source, "
                      ":refl_data_source, :UUID)");
    }

    // Bind values to the insert statement
    query.bindValue(":projectID", ProjectManager::getInstance().getPrjID());
    query.bindValue(":table_id", terminal->table_id.toInt());
    query.bindValue(":model", terminal->model);
    query.bindValue(":terminal_shape", terminal->terminal_shape);
    query.bindValue(":terminal_size", terminal->terminal_size);
    query.bindValue(":brand", terminal->brand);
    query.bindValue(":nosie_json", noiseJsonString);  // Note the typo in column name 'nosie_json' should match the table's column
    query.bindValue(":atten_json", attenJsonString);
    query.bindValue(":refl_json", reflJsonString);
    query.bindValue(":noi_data_source", terminal->noi_data_source);
    query.bindValue(":atten_data_source", terminal->atten_data_source);
    query.bindValue(":refl_data_source", terminal->refl_data_source);
    query.bindValue(":UUID", terminal->UUID);

    // Execute the insert operation
    if (!query.exec()) {
        qWarning() << "Insert operation failed:" << query.lastError();
        return false;
    }

    return true;
}

bool addOrUpdateOtherSendTerminalToDatabase(const ComponentBase& component, QSqlDatabase& db, bool update)
{
    // Check if the database is open
    if (!db.isOpen()) {
      qWarning() << "Database is not open!";
      return false;
    }

    // Attempt to cast ComponentBase to Other_send_terminal
    const Other_send_terminal* otherSendTerminal = dynamic_cast<const Other_send_terminal*>(&component);
    if (!otherSendTerminal) {
      qWarning() << "Component is not an Other_send_terminal";
      return false;
    }

    // Convert arrays to JSON
    QJsonDocument noiJson(QJsonArray::fromStringList(QList<QString>::fromStdList(std::list<QString>(otherSendTerminal->noi.begin(), otherSendTerminal->noi.end()))));
    QString noiseJsonString = noiJson.toJson(QJsonDocument::Compact);

    QJsonDocument attenJson(QJsonArray::fromStringList(QList<QString>::fromStdList(std::list<QString>(otherSendTerminal->atten.begin(), otherSendTerminal->atten.end()))));
    QString attenJsonString = attenJson.toJson(QJsonDocument::Compact);

    QJsonDocument reflJson(QJsonArray::fromStringList(QList<QString>::fromStdList(std::list<QString>(otherSendTerminal->refl.begin(), otherSendTerminal->refl.end()))));
    QString reflJsonString = reflJson.toJson(QJsonDocument::Compact);

    // Prepare SQL insert statement
    QSqlQuery query(db);
    if (update) {
        query.prepare("UPDATE other_send_terminal "
                      "SET projectID=:projectID, table_id=:table_id, model=:model, terminal_shape=:terminal_shape, "
                      "terminal_size=:terminal_size, brand=:brand, noise_json=:noise_json, atten_json=:atten_json, "
                      "refl_json=:refl_json, noi_data_source=:noi_data_source, atten_data_source=:atten_data_source, "
                      "refl_data_source=:refl_data_source, remark=:remark "
                      "WHERE UUID=:UUID");
    } else {
        query.prepare("INSERT INTO other_send_terminal (projectID, table_id, model, terminal_shape, "
                      "terminal_size, brand, nosie_json, atten_json, refl_json, noi_data_source, "
                      "atten_data_source, refl_data_source, remark, UUID) "
                      "VALUES (:projectID, :table_id, :model, :terminal_shape, :terminal_size, "
                      ":brand, :noise_json, :atten_json, :refl_json, :noi_data_source, :atten_data_source, "
                      ":refl_data_source, :remark, :UUID)");
    }

    // Bind values to the insert statement
    query.bindValue(":projectID", ProjectManager::getInstance().getPrjID());
    query.bindValue(":table_id", otherSendTerminal->table_id);
    query.bindValue(":model", otherSendTerminal->model);
    query.bindValue(":terminal_shape", otherSendTerminal->terminal_shape);
    query.bindValue(":terminal_size", otherSendTerminal->terminal_size);
    query.bindValue(":brand", otherSendTerminal->brand);
    query.bindValue(":noise_json", noiseJsonString);
    query.bindValue(":atten_json", attenJsonString);
    query.bindValue(":refl_json", reflJsonString);
    query.bindValue(":noi_data_source", otherSendTerminal->noi_data_source);
    query.bindValue(":atten_data_source", otherSendTerminal->atten_data_source);
    query.bindValue(":refl_data_source", otherSendTerminal->refl_data_source);
    query.bindValue(":remark", otherSendTerminal->remark);
    query.bindValue(":UUID", otherSendTerminal->UUID);

    // Execute the insert operation
    if (!query.exec()) {
      qWarning() << "Insert operation failed:" << query.lastError();
      return false;
    }

    return true;
}

bool addOrUpdateStaticBoxToDatabase(const ComponentBase& component, QSqlDatabase& db, bool update)
{
    // Check if the database is open
    if (!db.isOpen()) {
      qWarning() << "Database is not open!";
      return false;
    }

    // Attempt to cast ComponentBase to Branch
    const Branch* branch = dynamic_cast<const Branch*>(&component);
    if (!branch) {
      qWarning() << "Component is not a Branch";
      return false;
    }

    // Convert atten array to JSON
    QJsonDocument attenJson(QJsonArray::fromStringList(QList<QString>::fromStdList(std::list<QString>(branch->atten.begin(), branch->atten.end()))));
    QString attenJsonString = attenJson.toJson(QJsonDocument::Compact);

    // Prepare SQL insert statement
    QSqlQuery query(db);
    if (update) {
        query.prepare("UPDATE static_box "
                      "SET projectID=:projectID, table_id=:table_id, model=:model, brand=:brand, "
                      "q=:q, q1=:q1, atten_json=:atten_json, data_source=:data_source "
                      "WHERE UUID=:UUID");
    } else {
        query.prepare("INSERT INTO static_box (projectID, table_id, model, brand, q, q1, atten_json, data_source, UUID) "
                      "VALUES (:projectID, :table_id, :model, :brand, :q, :q1, :atten_json, :data_source, :UUID)");
    }

    // Bind values to the insert statement
    query.bindValue(":projectID", ProjectManager::getInstance().getPrjID());
    query.bindValue(":table_id", branch->table_id);
    query.bindValue(":model", branch->model);
    query.bindValue(":brand", branch->brand);
    query.bindValue(":q", branch->q);
    query.bindValue(":q1", branch->q1);
    query.bindValue(":atten_json", attenJsonString);
    query.bindValue(":data_source", branch->data_source);
    query.bindValue(":UUID", branch->UUID);

    // Execute the insert operation
    if (!query.exec()) {
      qWarning() << "Insert operation failed:" << query.lastError();
      return false;
    }

    return true;
}

bool addOrUpdateMultiRancToDatabase(const ComponentBase& component, QSqlDatabase& db, bool update)
{
    // Check if the database is open
    if (!db.isOpen()) {
      qWarning() << "Database is not open!";
      return false;
    }

    // Attempt to cast ComponentBase to Branch
    const Branch* branch = dynamic_cast<const Branch*>(&component);
    if (!branch) {
      qWarning() << "Component is not a Branch";
      return false;
    }

    // Convert atten array to JSON
    QJsonDocument attenJson(QJsonArray::fromStringList(QList<QString>::fromStdList(std::list<QString>(branch->atten.begin(), branch->atten.end()))));
    QString attenJsonString = attenJson.toJson(QJsonDocument::Compact);

    // Prepare SQL insert statement
    QSqlQuery query(db);
    if (update) {
        query.prepare("UPDATE multi_ranc "
                      "SET projectID=:projectID, table_id=:table_id, model=:model, brand=:brand, "
                      "q=:q, q1=:q1, atten_json=:atten_json, data_source=:data_source "
                      "WHERE UUID=:UUID");
    } else {
        query.prepare("INSERT INTO multi_ranc (projectID, table_id, model, brand, q, q1, atten_json, data_source, UUID) "
                      "VALUES (:projectID, :table_id, :model, :brand, :q, :q1, :atten_json, :data_source, :UUID)");
    }

    // Bind values to the insert statement
    query.bindValue(":projectID", ProjectManager::getInstance().getPrjID());
    query.bindValue(":table_id", component.table_id);
    query.bindValue(":model", branch->model);
    query.bindValue(":brand", branch->brand);
    query.bindValue(":q", branch->q);
    query.bindValue(":q1", branch->q1);
    query.bindValue(":atten_json", attenJsonString);
    query.bindValue(":data_source", component.data_source);
    query.bindValue(":UUID", component.UUID);

    // Execute the insert operation
    if (!query.exec()) {
      qWarning() << "Insert operation failed:" << query.lastError();
      return false;
    }

    return true;
}

bool addOrUpdateTeeToDatabase(const ComponentBase& component, QSqlDatabase& db, bool update)
{
    // Check if the database is open
    if (!db.isOpen()) {
      qWarning() << "Database is not open!";
      return false;
    }

    // Attempt to cast ComponentBase to Branch
    const Branch* branch = dynamic_cast<const Branch*>(&component);
    if (!branch) {
      qWarning() << "Component is not a Branch";
      return false;
    }

    // Convert atten array to JSON
    QJsonDocument attenJson(QJsonArray::fromStringList(QList<QString>::fromStdList(std::list<QString>(branch->atten.begin(), branch->atten.end()))));
    QString attenJsonString = attenJson.toJson(QJsonDocument::Compact);

    // Prepare SQL insert statement
    QSqlQuery query(db);
    if (update) {
        query.prepare("UPDATE tee "
                      "SET projectID=:projectID, table_id=:table_id, model=:model, brand=:brand, "
                      "q=:q, q1=:q1, atten_json=:atten_json, data_source=:data_source "
                      "WHERE UUID=:UUID");
    } else {
        query.prepare("INSERT INTO tee (projectID, table_id, model, brand, q, q1, atten_json, data_source, UUID) "
                      "VALUES (:projectID, :table_id, :model, :brand, :q, :q1, :atten_json, :data_source, :UUID)");
    }

    // Bind values to the insert statement
    query.bindValue(":projectID", ProjectManager::getInstance().getPrjID());
    query.bindValue(":table_id", component.table_id);
    query.bindValue(":model", branch->model);
    query.bindValue(":brand", branch->brand);
    query.bindValue(":q", branch->q);
    query.bindValue(":q1", branch->q1);
    query.bindValue(":atten_json", attenJsonString);
    query.bindValue(":data_source", component.data_source);
    query.bindValue(":UUID", component.UUID);

    // Execute the insert operation
    if (!query.exec()) {
      qWarning() << "Insert operation failed:" << query.lastError();
      return false;
    }

    return true;
}

bool addOrUpdatePipeToDatabase(const ComponentBase& component, QSqlDatabase& db, bool update)
{
    // Check if the database is open
    if (!db.isOpen()) {
      qWarning() << "Database is not open!";
      return false;
    }

    // Attempt to cast ComponentBase to Pipe
    const Pipe* pipe = dynamic_cast<const Pipe*>(&component);
    if (!pipe) {
      qWarning() << "Component is not a Pipe";
      return false;
    }

    // Convert atten array to JSON
    QJsonDocument attenJson(QJsonArray::fromStringList(QList<QString>::fromStdList(std::list<QString>(pipe->atten.begin(), pipe->atten.end()))));
    QString attenJsonString = attenJson.toJson(QJsonDocument::Compact);

    // Prepare SQL insert statement
    QSqlQuery query(db);
    if (update) {
        query.prepare("UPDATE pipe "
                      "SET projectID = :projectID, table_id = :table_id, model = :model, brand = :brand, "
                      "pipe_shape = :pipe_shape, size = :size, atten_json = :atten_json, data_source = :data_source "
                      "WHERE UUID = :UUID");
    } else {
        query.prepare("INSERT INTO pipe (projectID, table_id, model, brand, pipe_shape, size, atten_json, data_source, UUID) "
                      "VALUES (:projectID, :table_id, :model, :brand, :pipe_shape, :size, :atten_json, :data_source, :UUID)");
    }

    // Bind values to the insert statement
    query.bindValue(":projectID", ProjectManager::getInstance().getPrjID());
    query.bindValue(":table_id", component.table_id);
    query.bindValue(":model", pipe->model);
    query.bindValue(":brand", pipe->brand);
    query.bindValue(":pipe_shape", pipe->pipe_shape);
    query.bindValue(":size", pipe->size);
    query.bindValue(":atten_json", attenJsonString);
    query.bindValue(":data_source", component.data_source);
    query.bindValue(":UUID", component.UUID);

    // Execute the insert operation
    if (!query.exec()) {
      qWarning() << "Insert operation failed:" << query.lastError();
      return false;
    }

    return true;
}

bool addOrUpdateElbowToDatabase(const ComponentBase& component, QSqlDatabase& db, bool update)
{
    // Check if the database is open
    if (!db.isOpen()) {
      qWarning() << "Database is not open!";
      return false;
    }

    // Attempt to cast ComponentBase to Elbow
    const Elbow* elbow = dynamic_cast<const Elbow*>(&component);
    if (!elbow) {
      qWarning() << "Component is not an Elbow";
      return false;
    }

    // Convert atten array to JSON
    QJsonDocument attenJson(QJsonArray::fromStringList(QList<QString>::fromStdList(std::list<QString>(elbow->atten.begin(), elbow->atten.end()))));
    QString attenJsonString = attenJson.toJson(QJsonDocument::Compact);

    // Prepare SQL insert statement
    QSqlQuery query(db);
    if (update) {
        query.prepare("UPDATE elbow "
                      "SET projectID = :projectID, table_id = :table_id, model = :model, brand = :brand, "
                      "elbow_shape = :elbow_shape, size = :size, atten_json = :atten_json, data_source = :data_source "
                      "WHERE UUID = :UUID");
    } else {
        query.prepare("INSERT INTO elbow (projectID, table_id, model, brand, elbow_shape, size, atten_json, "
                      "data_source, UUID) "
                      "VALUES (:projectID, :table_id, :model, :brand, :elbow_shape, :size, :atten_json, "
                      ":data_source, :UUID)");
    }




    // Bind values to the insert statement
    query.bindValue(":projectID", ProjectManager::getInstance().getPrjID());
    query.bindValue(":table_id", component.table_id);
    query.bindValue(":model", elbow->model);
    query.bindValue(":brand", elbow->brand);
    query.bindValue(":elbow_shape", elbow->elbow_shape);
    query.bindValue(":size", elbow->size);
    query.bindValue(":atten_json", attenJsonString);
    query.bindValue(":data_source", component.data_source);
    query.bindValue(":UUID", component.UUID);

    // Execute the insert operation
    if (!query.exec()) {
      qWarning() << "Insert operation failed:" << query.lastError();
      return false;
    }

    return true;
}

bool addOrUpdateReducerToDatabase(const ComponentBase& component, QSqlDatabase& db, bool update)
{
    // Check if the database is open
    if (!db.isOpen()) {
      qWarning() << "Database is not open!";
      return false;
    }

    // Attempt to cast ComponentBase to Reducer
    const Reducer* reducer = dynamic_cast<const Reducer*>(&component);
    if (!reducer) {
      qWarning() << "Component is not a Reducer";
      return false;
    }

    // Convert atten array to JSON
    QJsonDocument attenJson(QJsonArray::fromStringList(QList<QString>::fromStdList(std::list<QString>(reducer->atten.begin(), reducer->atten.end()))));
    QString attenJsonString = attenJson.toJson(QJsonDocument::Compact);

    // Prepare SQL insert statement
    QSqlQuery query(db);
    if (update) {
        query.prepare("UPDATE reducer "
                      "SET projectID = :projectID, table_id = :table_id, model = :model, brand = :brand, "
                      "reducer_type = :reducer_type, reducer_before_size = :reducer_before_size, "
                      "reducer_after_size = :reducer_after_size, atten_json = :atten_json, data_source = :data_source "
                      "WHERE UUID = :UUID");
    } else {
        query.prepare("INSERT INTO reducer (projectID, table_id, model, brand, reducer_type, reducer_before_size, "
                      "reducer_after_size, atten_json, data_source, UUID) "
                      "VALUES (:projectID, :table_id, :model, :brand, :reducer_type, :reducer_before_size, "
                      ":reducer_after_size, :atten_json, :data_source, :UUID)");
    }

    // Bind values to the insert statement
    query.bindValue(":projectID", ProjectManager::getInstance().getPrjID());
    query.bindValue(":table_id", component.table_id);
    query.bindValue(":model", reducer->model);
    query.bindValue(":brand", reducer->brand);
    query.bindValue(":reducer_type", reducer->reducer_type);
    query.bindValue(":reducer_before_size", reducer->reducer_before_size);
    query.bindValue(":reducer_after_size", reducer->reducer_after_size);
    query.bindValue(":atten_json", attenJsonString);
    query.bindValue(":data_source", component.data_source);
    query.bindValue(":UUID", component.UUID);

    // Execute the insert operation
    if (!query.exec()) {
      qWarning() << "Insert operation failed:" << query.lastError();
      return false;
    }

    return true;
}

bool addOrUpdateSilencerToDatabase(const ComponentBase& component, QSqlDatabase& db, bool update)
{
    // Check if the database is open
    if (!db.isOpen()) {
      qWarning() << "Database is not open!";
      return false;
    }

    // Attempt to cast ComponentBase to Silencer
    const Silencer* silencer = dynamic_cast<const Silencer*>(&component);
    if (!silencer) {
      qWarning() << "Component is not a Silencer";
      return false;
    }

    // Convert atten array to JSON
    QJsonDocument attenJson(QJsonArray::fromStringList(QList<QString>::fromStdList(std::list<QString>(silencer->atten.begin(), silencer->atten.end()))));
    QString attenJsonString = attenJson.toJson(QJsonDocument::Compact);

    // Prepare SQL insert statement
    QSqlQuery query(db);
    if (update) {
        query.prepare("UPDATE silencer "
                      "SET projectID = :projectID, table_id = :table_id, model = :model, brand = :brand, "
                      "silencer_type = :silencer_type, atten_json = :atten_json, data_source = :data_source "
                      "WHERE UUID = :UUID");
    } else {
        query.prepare("INSERT INTO silencer (projectID, table_id, model, brand, silencer_type, atten_json, data_source, UUID) "
                      "VALUES (:projectID, :table_id, :model, :brand, :silencer_type, :atten_json, :data_source, :UUID)");
    }

    // Bind values to the insert statement
    query.bindValue(":projectID", ProjectManager::getInstance().getPrjID());
    query.bindValue(":table_id", component.table_id);
    query.bindValue(":model", silencer->model);
    query.bindValue(":brand", silencer->brand);
    query.bindValue(":silencer_type", silencer->silencer_type);
    query.bindValue(":atten_json", attenJsonString);
    query.bindValue(":data_source", component.data_source);
    query.bindValue(":UUID", component.UUID);

    // Execute the insert operation
    if (!query.exec()) {
      qWarning() << "Insert operation failed:" << query.lastError();
      return false;
    }

    return true;
}

}

namespace DBComponentDelOperations {
bool deleteComponentFromDatabase(const QString &tableName, const QString &UUID, QSqlDatabase &db)
{
    if (!db.isOpen()) {
        // 确保数据库已经打开
        qWarning() << "Database is not open!";
        return false;
    }

    // 创建并准备 SQL 删除语句
    QSqlQuery query(db);
    QString sql = QString("DELETE FROM %1 WHERE UUID = :UUID").arg(tableName);
    query.prepare(sql);

    // 绑定UUID值到删除语句
    query.bindValue(":UUID", UUID);

    // 执行 SQL 删除操作
    if (!query.exec()) {
        qWarning() << "Delete operation failed:" << query.lastError();
        return false;
    }

    return true;
}
}
