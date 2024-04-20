#ifndef COMPONENTSTRUCTS_H
#define COMPONENTSTRUCTS_H

#include <QString>
#include <QObject>
#include <array>
#include <memory>
#include <QUuid>
#include <QJsonDocument>
#include <QJsonArray>
#include <QSqlRecord>
#include <QVariant>

using std::array;
using std::unique_ptr;

namespace component_type_name{
const QString FAN = QStringLiteral("风机");
const QString FANCOIL = QStringLiteral("风机盘管");
const QString AIRCONDITION = QStringLiteral("空调器");
const QString VAV_TERMINAL = QStringLiteral("变风量末端");
const QString CIRCULAR_DAMPER = QStringLiteral("圆形调风门");
const QString RECT_DAMPER = QStringLiteral("方形调风门");
const QString AIRDIFF = QStringLiteral("布风器+散流器");
const QString PUMPSEND = QStringLiteral("抽/送风头");
const QString STATICBOX_GRILLE = QStringLiteral("静压箱+格栅");
const QString DISP_VENT_TERMINAL = QStringLiteral("置换通风末端");
const QString OTHER_SEND_TERMINAL = QStringLiteral("其他送风末端");
const QString STATICBOX = QStringLiteral("静压箱");
const QString MULTI_RANC = QStringLiteral("风道多分支");
const QString TEE = QStringLiteral("三通");
const QString PIPE = QStringLiteral("直管");
const QString ELBOW = QStringLiteral("弯头");
const QString REDUCER = QStringLiteral("变径");
const QString SILENCER = QStringLiteral("消音器");
}

class ComponentBase
{
public:
    QString model;  //型号
    QString brand;  //品牌
    QString table_id;   //表格编号
    QString UUID; //识别码
    QString data_source;    //来源

    virtual QString typeName() const = 0;
    virtual QList<QStringList> getComponentDataAsStringList() const = 0;

    // 构造函数
    ComponentBase(const QString& model, const QString& brand, const QString& table_id,
                  const QString& data_source, const QString& UUID = "")
        : model(model), brand(brand), table_id(table_id), UUID(UUID), data_source(data_source)
    {
        this->UUID = UUID.isEmpty() ? QUuid::createUuid().toString(QUuid::WithoutBraces) : UUID;
    }

    // 默认构造函数（如果需要的话，允许创建没有初始信息的对象）
    ComponentBase() = default;

    // 虚析构函数，保证派生类的正确析构
    virtual ~ComponentBase() = default;

    void setTableID(const QString& table_id)
    {
        this->table_id = table_id;
    }

    array<QString, 9> deserializeNoise(const QString& jsonString) {
        array<QString, 9> noiseArray;
        noiseArray.fill(""); // 使用空字符串初始化数组

        QJsonDocument doc = QJsonDocument::fromJson(jsonString.toUtf8());
        if (!doc.isNull() && doc.isArray()) {
            QJsonArray jsonArray = doc.array();
            int index = 0;
            for (const auto& value : jsonArray) {
                if (index >= noiseArray.size()) {
                    break; // 防止数组越界
                }
                noiseArray[index] = value.toString();
                ++index;
            }
        }

        return noiseArray;
    }

    array<QString, 8> deserializeAtten(const QString& jsonString) {
        array<QString, 8> attenArray;
        attenArray.fill(""); // 使用空字符串初始化数组

        QJsonDocument doc = QJsonDocument::fromJson(jsonString.toUtf8());
        if (!doc.isNull() && doc.isArray()) {
            QJsonArray jsonArray = doc.array();
            int index = 0;
            for (const auto& value : jsonArray) {
                if (index >= attenArray.size()) {
                    break; // 防止数组越界
                }
                attenArray[index] = value.toString();
                ++index;
            }
        }

        return attenArray;
    }
};

class Terminal : public ComponentBase
{
public:
    QString terminal_shape;   // 类型 圆还是方
    QString terminal_size;
    array<QString, 9> noi;  // 噪音
    array<QString, 8> atten;  // 衰减
    array<QString, 8> refl;  // 反射衰减

    QString noi_data_source;    //来源
    QString atten_data_source;    //来源
    QString refl_data_source;    //来源

    virtual QString typeName() const = 0;
    virtual QList<QStringList> getComponentDataAsStringList() const = 0;

    Terminal() = default;

    Terminal(const QString& model, const QString& brand, const QString& table_id,
             const QString& UUID, const QString& noi_data_source,
             const QString& atten_data_source, const QString& refl_data_source,
             const QString& shape, const QString& terminal_size,
             const array<QString, 9>& noi, const array<QString, 8>& atten,
             const array<QString, 8>& refl)
        :   ComponentBase(model, brand, table_id, "", UUID),
          terminal_shape(shape),
          terminal_size(terminal_size),
          noi(noi),
          atten(atten),
          refl(refl),noi_data_source(noi_data_source),atten_data_source(atten_data_source),refl_data_source(refl_data_source)
    {
    }

    Terminal(const QString& model, const QString& brand, const QString& table_id,
             const QString& UUID, const QString& noi_data_source,
             const QString& atten_data_source, const QString& refl_data_source,
             const QString& shape, const QString& terminal_size)
        :   ComponentBase(model, brand, table_id, data_source, UUID),
          terminal_shape(shape),
          terminal_size(terminal_size),
          noi_data_source(noi_data_source),atten_data_source(atten_data_source),refl_data_source(refl_data_source)
    {
    }
};

class Valve : public ComponentBase
{
public:
    QString angle;   //阀门开度
    QString air_volume;   //风量
    array<QString, 9> noi;  // 噪音

    virtual QString typeName() const = 0;
    virtual QList<QStringList> getComponentDataAsStringList() const = 0;

    Valve() = default;

    Valve(const QString& model, const QString& brand, const QString& table_id,
          const QString& data_source, const QString& UUID,
          const QString& angle, const QString& air_volume,
          const array<QString, 9>& noi)
        : ComponentBase(model, brand, table_id, data_source, UUID),
          angle(angle),
          air_volume(air_volume),
          noi(noi)
    {
    }

    Valve(const QString& model, const QString& brand, const QString& table_id,
          const QString& data_source, const QString& UUID,
          const QString& angle, const QString& air_volume)
        : ComponentBase(model, brand, table_id, data_source, UUID),
          angle(angle),
          air_volume(air_volume)
    {
    }
};

class Branch : public ComponentBase
{
public:
    QString q1;  //总风量
    QString q;   //支管风量
    array<QString, 8> atten;  //衰减

    virtual QString typeName() const = 0;
    virtual QList<QStringList> getComponentDataAsStringList() const = 0;

    Branch() = default;

    // 构造函数，包含atten数组参数
    Branch(const QString& model, const QString& brand, const QString& table_id,
           const QString& UUID, const QString& data_source,
           const QString& q1, const QString& q, const array<QString, 8>& atten)
        : ComponentBase(model, brand, table_id, data_source, UUID),
          q1(q1),
          q(q),
          atten(atten)
    {
    }

    // 构造函数，不包含atten数组参数
    Branch(const QString& model, const QString& brand, const QString& table_id,
           const QString& UUID, const QString& data_source,
           const QString& q1, const QString& q)
        : ComponentBase(model, brand, table_id, data_source, UUID),
          q1(q1),
          q(q)
    {
    }
};

typedef struct Fan : ComponentBase
{
    QString number;  //编号
    QString air_volume; //风量
    QString static_pressure; //静压
    array<QString, 9> noi_in;      //送风机进口噪音
    array<QString, 9> noi_out;     //送风机出口噪音

    Fan(const QSqlRecord& record) {
        this->model = record.value("model").toString();
        this->brand = record.value("brand").toString();
        this->table_id = record.value("table_id").toString();
        this->UUID = record.value("UUID").toString();
        this->data_source = record.value("data_source").toString();
        this->number = record.value("number").toString();
        this->air_volume = record.value("air_volume").toString();
        this->static_pressure = record.value("static_pressure").toString();
        this->noi_in = deserializeNoise(record.value("noise_in_json").toString());
        this->noi_out = deserializeNoise(record.value("noise_out_json").toString());
    }

    // 构造函数，包含noi_in和noi_out数组参数
    Fan(const QString& model, const QString& brand, const QString& table_id,
        const QString& UUID, const QString& data_source, const QString& number,
        const QString& air_volume, const QString& static_pressure,
        const array<QString, 9>& noi_in, const array<QString, 9>& noi_out)
        : ComponentBase(model, brand, table_id, data_source, UUID),  // 初始化基类部分
          number(number),
          air_volume(air_volume),
          static_pressure(static_pressure),
          noi_in(noi_in),
          noi_out(noi_out)
    {
    }

    // 构造函数，不包含noi_in和noi_out数组参数
    Fan(const QString& model = QString(), const QString& brand = QString(),
        const QString& table_id = QString(), const QString& UUID = QString(),
        const QString& data_source = QString(), const QString& number = QString(),
        const QString& air_volume = QString(), const QString& static_pressure = QString())
        : ComponentBase(model, brand, table_id, data_source, UUID), // 初始化基类部分
          number(number),
          air_volume(air_volume),
          static_pressure(static_pressure)
    {
    }

    // ComponentBase interface
public:
    QString typeName() const override{
        return component_type_name::FAN;
    }

    // ComponentBase interface
public:
    QList<QStringList> getComponentDataAsStringList() const override
    {
        QList<QStringList> dataLists;
        QStringList data_in = {
            table_id,
            number,
            model,
            brand,
            air_volume,
            static_pressure,
            "进口",
        };

        for (const auto& noi_value : noi_in) {
            data_in.push_back(noi_value);
        }

        data_in.push_back(data_source);
        data_in.push_back(UUID);

        QStringList data_out = {
            table_id,
            number,
            model,
            brand,
            air_volume,
            static_pressure,
            "出口",
        };

        // 迭代 noi_out 数组来填充 QStringList
        for (const auto& noi_value : noi_out) {
            data_out.push_back(noi_value);
        }

        data_out.push_back(data_source);
        data_out.push_back(UUID);

        dataLists.append(data_in);
        dataLists.append(data_out);

        return dataLists;
    }
}Fan;

typedef struct FanCoil : ComponentBase
{
    QString air_volume;         // 风量
    QString static_pressure;    // 静压
    QString type;               // 类型
    array<QString, 9> noi_in;   // 送风机进口噪音
    array<QString, 9> noi_out;  // 送风机出口噪音

    // 默认构造函数
    FanCoil() = default;

    FanCoil(const QSqlRecord& record) : ComponentBase() {
        // Base class member variables
        this->model = record.value("model").toString();
        this->brand = record.value("brand").toString();
        this->table_id = record.value("table_id").toString();
        this->UUID = record.value("UUID").toString();
        this->data_source = record.value("data_source").toString();

        // FanCoil specific member variables
        this->type = record.value("type").toString();
        this->air_volume = record.value("air_volume").toString();
        this->static_pressure = record.value("static_pressure").toString();

        // Handle noise data (assuming `deserializeNoise` function exists)
        QString noiseInJson = record.value("noise_in_json").toString();
        this->noi_in = deserializeNoise(noiseInJson);

        QString noiseOutJson = record.value("noise_out_json").toString();
        this->noi_out = deserializeNoise(noiseOutJson);
    }

    // 包含所有成员变量的构造函数
    FanCoil(const QString& model, const QString& brand, const QString& table_id,
            const QString& UUID, const QString& data_source,
            const QString& air_volume, const QString& static_pressure,
            const QString& type, const array<QString, 9>& noi_in,
            const array<QString, 9>& noi_out)
        : ComponentBase(model, brand, table_id, data_source, UUID),
          air_volume(air_volume), static_pressure(static_pressure),
          type(type), noi_in(noi_in), noi_out(noi_out)
    {
    }

    // 不包含noi_in和noi_out数组的构造函数
    FanCoil(const QString& model, const QString& brand, const QString& table_id,
            const QString& UUID, const QString& data_source,
            const QString& air_volume, const QString& static_pressure,
            const QString& type)
        : ComponentBase(model, brand, table_id, data_source, UUID),
          air_volume(air_volume), static_pressure(static_pressure), type(type)
    {
    }

    // ComponentBase interface
public:
    QString typeName() const override{
        return component_type_name::FANCOIL;
    }

    // ComponentBase interface
public:
    QList<QStringList> getComponentDataAsStringList() const override
    {
        QList<QStringList> dataLists;
        QStringList data_in = {
            table_id,
            type,
            model,
            brand,
            air_volume,
            static_pressure,
            "进口"
        };

        for (const auto& noi_value : noi_in) {
            data_in.push_back(noi_value);
        }

        data_in.push_back(data_source);
        data_in.push_back(UUID);

        QStringList data_out = {
            table_id,
            type,
            model,
            brand,
            air_volume,
            static_pressure,
            "出口"
        };

        // 迭代 noi_out 数组来填充 QStringList
        for (const auto& noi_value : noi_out) {
            data_out.push_back(noi_value);
        }

        data_out.push_back(data_source);
        data_out.push_back(UUID);

        dataLists.append(data_in);
        dataLists.append(data_out);

        return dataLists;
    }
} FanCoil;

typedef struct Aircondition : ComponentBase
{
    int fan_counts;  // 风机数量

    QString send_number;  // 送风机编号
    QString send_air_volume; // 送风机风量
    QString send_static_pressure; // 送风机静压

    QString exhaust_number;  // 排风机编号
    QString exhaust_air_volume; // 送风机风量
    QString exhaust_static_pressure; // 排风机静压

    array<QString, 9> noi_send_in;      // 送风机进口噪音
    array<QString, 9> noi_send_out;     // 送风机出口噪音
    array<QString, 9> noi_exhaust_in;   // 排风机进口噪音
    array<QString, 9> noi_exhaust_out;  // 排风机出口噪音

    // 默认构造函数
    Aircondition() = default;

    Aircondition(const QSqlRecord& record) : ComponentBase() {
        // Base class member variables
        this->model = record.value("model").toString();
        this->brand = record.value("brand").toString();
        this->table_id = record.value("table_id").toString();
        this->UUID = record.value("UUID").toString();
        this->data_source = record.value("data_source").toString();

        // Aircondition specific member variables
        this->fan_counts = record.value("fan_counts").toInt();
        this->send_number = record.value("send_number").toString();
        this->send_air_volume = record.value("send_air_volume").toString();
        this->send_static_pressure = record.value("send_static_pressure").toString();
        this->exhaust_number = record.value("exhaust_number").toString();
        this->exhaust_air_volume = record.value("exhaust_air_volume").toString();
        this->exhaust_static_pressure = record.value("exhaust_static_pressure").toString();

        // Handle noise data (assuming `deserializeNoise` function exists)
        QString noiseSendInJson = record.value("noise_send_in_json").toString();
        this->noi_send_in = deserializeNoise(noiseSendInJson);

        QString noiseSendOutJson = record.value("noise_send_out_json").toString();
        this->noi_send_out = deserializeNoise(noiseSendOutJson);

        QString noiseExhaustInJson = record.value("noise_exhaust_in_json").toString();
        if (!noiseExhaustInJson.isEmpty()) {
            this->noi_exhaust_in = deserializeNoise(noiseExhaustInJson);
        }

        QString noiseExhaustOutJson = record.value("noise_exhaust_out_json").toString();
        if (!noiseExhaustOutJson.isEmpty()) {
            this->noi_exhaust_out = deserializeNoise(noiseExhaustOutJson);
        }
    }

    // 包含所有成员变量的构造函数
    Aircondition(const QString& model, const QString& brand, const QString& table_id,
                 const QString& UUID, const QString& data_source,
                 int fan_counts, const QString& send_number, const QString& send_air_volume,
                 const QString& send_static_pressure,const array<QString, 9>& noi_send_in,
                 const array<QString, 9>& noi_send_out)
        : ComponentBase(model, brand, table_id, data_source, UUID),
          fan_counts(fan_counts), send_number(send_number), send_air_volume(send_air_volume),
          send_static_pressure(send_static_pressure),
          noi_send_in(noi_send_in), noi_send_out(noi_send_out)
    {
    }

    // 包含所有成员变量的构造函数
    Aircondition(const QString& model, const QString& brand, const QString& table_id,
                 const QString& UUID, const QString& data_source,
                 int fan_counts, const QString& send_number, const QString& send_air_volume,
                 const QString& send_static_pressure, const QString& exhaust_number,
                 const QString& exhaust_air_volume, const QString& exhaust_static_pressure,
                 const array<QString, 9>& noi_send_in, const array<QString, 9>& noi_send_out,
                 const array<QString, 9>& noi_exhaust_in, const array<QString, 9>& noi_exhaust_out)
        : ComponentBase(model, brand, table_id, data_source, UUID),
          fan_counts(fan_counts), send_number(send_number), send_air_volume(send_air_volume),
          send_static_pressure(send_static_pressure), exhaust_number(exhaust_number),
          exhaust_air_volume(exhaust_air_volume), exhaust_static_pressure(exhaust_static_pressure),
          noi_send_in(noi_send_in), noi_send_out(noi_send_out),
          noi_exhaust_in(noi_exhaust_in), noi_exhaust_out(noi_exhaust_out)
    {
    }

    // 不包含噪音数组的构造函数
    Aircondition(const QString& model, const QString& brand, const QString& table_id,
                 const QString& UUID, const QString& data_source,
                 int fan_counts, const QString& send_number, const QString& send_air_volume,
                 const QString& send_static_pressure, const QString& exhaust_number,
                 const QString& exhaust_air_volume, const QString& exhaust_static_pressure)
        : ComponentBase(model, brand, table_id, data_source, UUID),
          fan_counts(fan_counts), send_number(send_number), send_air_volume(send_air_volume),
          send_static_pressure(send_static_pressure), exhaust_number(exhaust_number),
          exhaust_air_volume(exhaust_air_volume), exhaust_static_pressure(exhaust_static_pressure)
    {
        // 在这个构造函数中，噪音相关的数组将保持它们的默认初始化状态
    }

    // ComponentBase interface
public:
    QString typeName() const override{
        return component_type_name::AIRCONDITION;
    }
    // ComponentBase interface
public:
    QList<QStringList> getComponentDataAsStringList() const override
    {
        QList<QStringList> dataLists;

        QStringList data_send_in = {
            table_id,
            send_number,
            model,
            brand,
            send_air_volume,
            send_static_pressure,
            "送风机",
            "进口",
        };

        for (const auto& noi_value : noi_send_in) {
            data_send_in.push_back(noi_value);
        }

        data_send_in.push_back(data_source);
        data_send_in.push_back(UUID);

        QStringList data_send_out = {
            table_id,
            send_number,
            model,
            brand,
            send_air_volume,
            send_static_pressure,
            "送风机",
            "出口",
        };

        for (const auto& noi_value : noi_send_out) {
            data_send_out.push_back(noi_value);
        }

        data_send_out.push_back(data_source);
        data_send_out.push_back(UUID);

        dataLists.append(data_send_in);
        dataLists.append(data_send_out);

        if(fan_counts == 2)
        {
            QStringList data_exhaust_in = {
                table_id,
                exhaust_number,
                model,
                brand,
                exhaust_air_volume,
                exhaust_static_pressure,
                "排风机",
                "进口",
            };

            for (const auto& noi_value : noi_exhaust_in) {
                data_exhaust_in.push_back(noi_value);
            }

            data_exhaust_in.push_back(data_source);
            data_exhaust_in.push_back(UUID);

            QStringList data_exhaust_out = {
                table_id,
                exhaust_number,
                model,
                brand,
                exhaust_air_volume,
                exhaust_static_pressure,
                "排风机",
                "出口",
            };

            for (const auto& noi_value : noi_exhaust_out) {
                data_exhaust_out.push_back(noi_value);
            }

            data_exhaust_out.push_back(data_source);
            data_exhaust_out.push_back(UUID);

            dataLists.append(data_exhaust_in);
            dataLists.append(data_exhaust_out);
        }

        return dataLists;
    }
} Aircondition;

typedef struct VAV_terminal : Valve
{
    QString number;  // 编号

    // 使用Valve的默认构造函数并初始化number
    VAV_terminal() = default;

    VAV_terminal(const QSqlRecord& record) {
        // Base class member variables (inherited from ComponentBase)
        this->model = record.value("model").toString();
        this->brand = record.value("brand").toString();
        this->table_id = record.value("table_id").toString();
        this->UUID = record.value("UUID").toString();
        this->data_source = record.value("data_source").toString();

        // VAV_terminal specific member variables
        this->number = record.value("number").toString();
        this->angle = record.value("valve_angle").toString(); // Assuming real is stored as double in QSqlRecord
        this->air_volume = record.value("air_volume").toString(); // Assuming real is stored as double in QSqlRecord

        // Handle noise data (assuming `deserializeNoise` function exists)
        QString noiseJson = record.value("noise_json").toString();
        this->noi = deserializeNoise(noiseJson);
    }

    // 使用Valve的全部参数构造函数初始化基类部分，并初始化number
    VAV_terminal(const QString& model, const QString& brand, const QString& table_id,
                 const QString& UUID, const QString& data_source,
                 const QString& angle, const QString& air_volume,
                 const array<QString, 9>& noi, const QString& number)
        : Valve(model, brand, table_id, data_source, UUID, angle, air_volume, noi),
          number(number) {}

    // 使用Valve的不包含噪音数组的构造函数初始化基类部分，并初始化number
    VAV_terminal(const QString& model, const QString& brand, const QString& table_id,
                 const QString& UUID, const QString& data_source,
                 const QString& angle, const QString& air_volume, const QString& number)
        : Valve(model, brand, table_id, data_source, UUID, angle, air_volume),
          number(number) {}

    // ComponentBase interface
public:
    QString typeName() const override{
        return component_type_name::VAV_TERMINAL;
    }

    // ComponentBase interface
public:
    QList<QStringList> getComponentDataAsStringList() const override
    {
        QList<QStringList> dataLists;
        QStringList data = {
            table_id,
            number,
            model,
            brand,
            angle,
            air_volume
        };

        // 迭代 noi_out 数组来填充 QStringList
        for (const auto& noi_value : noi) {
            data.push_back(noi_value);
        }

        data.push_back(data_source);
        data.push_back(UUID);

        dataLists.append(data);
        return dataLists;
    }
}VAV_terminal;

typedef struct Circular_damper : Valve
{
    // 使用Valve的默认构造函数
    QString diameter;

    Circular_damper() : Valve() {}

    Circular_damper(const QSqlRecord& record) {
        // Base class member variables (inherited from ComponentBase)
        this->model = record.value("model").toString();
        this->brand = record.value("brand").toString();
        this->table_id = record.value("table_id").toString();
        this->UUID = record.value("UUID").toString();
        this->data_source = record.value("data_source").toString();

        // VAV_terminal specific member variables
        this->diameter = record.value("size").toString();
        this->angle = record.value("valve_angle").toString(); // Assuming real is stored as double in QSqlRecord
        this->air_volume = record.value("air_volume").toString(); // Assuming real is stored as double in QSqlRecord

        // Handle noise data (assuming `deserializeNoise` function exists)
        QString noiseJson = record.value("noise_json").toString();
        this->noi = deserializeNoise(noiseJson);
    }

    // 使用Valve的全部参数构造函数初始化基类部分
    Circular_damper(const QString& model, const QString& brand, const QString& table_id,
                    const QString& UUID, const QString& data_source, const QString& diameter,
                    const QString& angle, const QString& air_volume,
                    const array<QString, 9>& noi)
        : Valve(model, brand, table_id, data_source, UUID, angle, air_volume, noi),
          diameter(diameter)
    {
    }

    // 使用Valve的不包含噪音数组的构造函数初始化基类部分
    Circular_damper(const QString& model, const QString& brand, const QString& table_id,
                    const QString& UUID, const QString& data_source, const QString& diameter,
                    const QString& angle, const QString& air_volume)
        : Valve(model, brand, table_id, data_source, UUID, angle, air_volume),
          diameter(diameter)
    {}

    // ComponentBase interface
public:
    QString typeName() const override{
        return component_type_name::CIRCULAR_DAMPER;
    }

    // ComponentBase interface
public:
    QList<QStringList> getComponentDataAsStringList() const override
    {
        QList<QStringList> dataLists;
        QStringList data = {
            table_id,
            model,
            brand,
            diameter,
            air_volume,
            angle,
        };

        // 迭代 noi_out 数组来填充 QStringList
        for (const auto& noi_value : noi) {
            data.push_back(noi_value);
        }

        data.push_back(data_source);
        data.push_back(UUID);

        dataLists.append(data);

        return dataLists;
    }
}Circular_damper;

typedef struct Rect_damper : Valve
{
    // 使用Valve的默认构造函数

    QString length;
    QString width;
    QString size;

    Rect_damper() : Valve() {}

    Rect_damper(const QSqlRecord& record) {
        // Base class member variables (inherited from ComponentBase)
        this->model = record.value("model").toString();
        this->brand = record.value("brand").toString();
        this->table_id = record.value("table_id").toString();
        this->UUID = record.value("UUID").toString();
        this->data_source = record.value("data_source").toString();

        // VAV_terminal specific member variables
        this->size = record.value("size").toString();
        this->angle = record.value("valve_angle").toString(); // Assuming real is stored as double in QSqlRecord
        this->air_volume = record.value("air_volume").toString(); // Assuming real is stored as double in QSqlRecord

        // Handle noise data (assuming `deserializeNoise` function exists)
        QString noiseJson = record.value("noise_json").toString();
        this->noi = deserializeNoise(noiseJson);
    }

    // 使用Valve的全部参数构造函数初始化基类部分
    Rect_damper(const QString& model, const QString& brand, const QString& table_id,
                const QString& UUID, const QString& data_source,
                const QString& angle, const QString& air_volume,
                const array<QString, 9>& noi, const QString& length,  const QString& width)
        : Valve(model, brand, table_id, data_source, UUID, angle, air_volume, noi),
          length(length), width(width), size(length + "x" + width) {}

    // 使用Valve的不包含噪音数组的构造函数初始化基类部分
    Rect_damper(const QString& model, const QString& brand, const QString& table_id,
                const QString& UUID, const QString& data_source,
                const QString& angle, const QString& air_volume,
                const QString& length,  const QString& width )
        : Valve(model, brand, table_id, data_source, UUID, angle, air_volume),
          length(length), width(width), size(length + "x" + width) {}

    // ComponentBase interface
public:
    QString typeName() const override{
        return component_type_name::RECT_DAMPER;
    }

    // ComponentBase interface
public:
    QList<QStringList> getComponentDataAsStringList() const override
    {
        QList<QStringList> dataLists;
        QStringList data = {
            table_id,
            model,
            brand,
            length + "x" + width,
            air_volume,
            angle,
        };

        // 迭代 noi_out 数组来填充 QStringList
        for (const auto& noi_value : noi) {
            data.push_back(noi_value);
        }

        data.push_back(data_source);
        data.push_back(UUID);

        dataLists.append(data);

        return dataLists;
    }
}Rect_damper;

typedef struct AirDiff : Terminal
{
    QString air_distributor_model;  // 布风器型号
    QString air_distributor_brand;  // 品牌
    QString diffuser_model;  // 散流器型号
    QString diffuser_brand;  // 品牌

    AirDiff() = default;

    AirDiff(const QSqlRecord& record) {
        // Base class member variables (inherited from ComponentBase)
        this->table_id = record.value("table_id").toString();
        this->UUID = record.value("UUID").toString();

        // Terminal specific member variables
        this->terminal_shape = record.value("terminal_shape").toString();
        this->terminal_size = record.value("terminal_size").toString();
        this->noi_data_source = record.value("noi_data_source").toString();
        this->atten_data_source = record.value("atten_data_source").toString();
        this->refl_data_source = record.value("refl_data_source").toString();

        // Handle noise data (assuming `deserializeNoise` function exists)
        QString noiseJson = record.value("noise_json").toString();
        this->noi = deserializeNoise(noiseJson);

        // Handle attenuation data (assuming `deserializeAttenuation` function exists)
        QString attenJson = record.value("atten_json").toString();
        this->atten = deserializeAtten(attenJson);

        // Handle reflection data (assuming `deserializeReflection` function exists)
        QString reflJson = record.value("refl_json").toString();
        this->refl = deserializeAtten(reflJson);

        // AirDiff specific member variables
        this->air_distributor_model = record.value("air_distributor_model").toString();
        this->air_distributor_brand = record.value("air_distributor_brand").toString();
        this->diffuser_model = record.value("diffuser_model").toString();
        this->diffuser_brand = record.value("diffuser_brand").toString();
    }

    AirDiff(const QString& model, const QString& brand, const QString& table_id,
            const QString& UUID, const QString& noi_data_source,
            const QString& atten_data_source, const QString& refl_data_source,
            const QString& shape, const QString& terminal_size,
            const array<QString, 9>& noi, const array<QString, 8>& atten,
            const array<QString, 8>& refl,
            const QString& air_distributor_model, const QString& air_distributor_brand,
            const QString& diffuser_model, const QString& diffuser_brand)
        : Terminal(model, brand, table_id, UUID, noi_data_source, atten_data_source, refl_data_source,
                   shape, terminal_size, noi, atten, refl),
          air_distributor_model(air_distributor_model),
          air_distributor_brand(air_distributor_brand),
          diffuser_model(diffuser_model),
          diffuser_brand(diffuser_brand)
    {
    }

    // 简化版构造函数，不包含噪音、衰减和反射衰减数组
    AirDiff(const QString& model, const QString& brand, const QString& table_id,
            const QString& UUID, const QString& noi_data_source,
            const QString& atten_data_source, const QString& refl_data_source,
            const QString& shape, const QString& terminal_size,
            const QString& air_distributor_model, const QString& air_distributor_brand,
            const QString& diffuser_model, const QString& diffuser_brand)
        : Terminal(model, brand, table_id, UUID, noi_data_source, atten_data_source, refl_data_source,
                   shape, terminal_size),
          air_distributor_model(air_distributor_model),
          air_distributor_brand(air_distributor_brand),
          diffuser_model(diffuser_model),
          diffuser_brand(diffuser_brand)
    {
    }

    // ComponentBase interface
public:
    QString typeName() const override{
        return component_type_name::AIRDIFF;
    }
    // ComponentBase interface
public:
    QList<QStringList> getComponentDataAsStringList() const override
    {
        QList<QStringList> dataLists;
        QStringList data_noise = {
            table_id,
            air_distributor_model,
            air_distributor_brand,
            diffuser_model,
            diffuser_brand,
            terminal_shape,
            terminal_size,
        };

        // 迭代 noi_out 数组来填充 QStringList
        for (const auto& noi_value : noi) {
            data_noise.push_back(noi_value);
        }

        data_noise.push_back(noi_data_source);
        data_noise.push_back(UUID);


        QStringList data_atten = {
            table_id,
            air_distributor_model,
            air_distributor_brand,
            diffuser_model,
            diffuser_brand,
            terminal_shape,
            terminal_size,
        };

        // 迭代 noi_out 数组来填充 QStringList
        for (const auto& noi_value : atten) {
            data_atten.push_back(noi_value);
        }

        data_atten.push_back(atten_data_source);
        data_atten.push_back(UUID);

        QStringList data_refl = {
            table_id,
            air_distributor_model,
            air_distributor_brand,
            diffuser_model,
            diffuser_brand,
            terminal_shape,
            terminal_size,
        };

        // 迭代 noi_out 数组来填充 QStringList
        for (const auto& noi_value : refl) {
            data_refl.push_back(noi_value);
        }

        data_refl.push_back(refl_data_source);
        data_refl.push_back(UUID);

        dataLists.append(data_noise);
        dataLists.append(data_atten);
        dataLists.append(data_refl);

        return dataLists;
    }
}AirDiff;

namespace pump_send_type {
const QString PUMP = QStringLiteral("抽风头");
const QString SEND = QStringLiteral("送风头");
}
typedef struct PumpSend : Terminal
{

    QString type_pump_or_send;  //类型 抽风头or送风头

    PumpSend() = default;

    PumpSend(const QSqlRecord& record) {
        // Base class member variables (inherited from ComponentBase)
        this->model = record.value("model").toString();
        this->brand = record.value("brand").toString();
        this->table_id = record.value("table_id").toString();
        this->UUID = record.value("UUID").toString();

        // Terminal specific member variables
        this->terminal_shape = record.value("terminal_shape").toString();
        this->terminal_size = record.value("terminal_size").toString();
        this->noi_data_source = record.value("noi_data_source").toString();
        this->atten_data_source = record.value("atten_data_source").toString();
        this->refl_data_source = record.value("refl_data_source").toString();


        // Handle noise data (assuming `deserializeNoise` function exists)
        QString noiseJson = record.value("noise_json").toString();
        this->noi = deserializeNoise(noiseJson);

        // Handle attenuation data (assuming `deserializeAttenuation` function exists)
        QString attenJson = record.value("atten_json").toString();
        this->atten = deserializeAtten(attenJson);

        // Handle reflection data (assuming `deserializeReflection` function exists)
        QString reflJson = record.value("refl_json").toString();
        this->refl = deserializeAtten(reflJson);

        // PumpSend specific member variable
        this->type_pump_or_send = record.value("type_pump_or_send").toString();
    }

    PumpSend(const QString& model, const QString& brand, const QString& table_id,
             const QString& UUID, const QString& noi_data_source,
             const QString& atten_data_source, const QString& refl_data_source,
             const QString& shape, const QString& size,
             const array<QString, 9>& noi, const array<QString, 8>& atten,
             const array<QString, 8>& refl,
             const QString& type)
        : Terminal(model, brand, table_id, UUID, noi_data_source, atten_data_source, refl_data_source,
                   shape, size, noi, atten, refl),
          type_pump_or_send(type)
    {
    }

    // 简化版构造函数，不包含噪音、衰减和反射衰减数组
    PumpSend(const QString& model, const QString& brand, const QString& table_id,
             const QString& UUID, const QString& noi_data_source,
             const QString& atten_data_source, const QString& refl_data_source,
             const QString& shape, const QString& size,
             const QString& type)
        : Terminal(model, brand, table_id, UUID, noi_data_source, atten_data_source, refl_data_source,
                   shape, size),
          type_pump_or_send(type)
    {
    }

    // ComponentBase interface
public:
    QString typeName() const override{
        return component_type_name::PUMPSEND;
    }

    // ComponentBase interface
public:
    QList<QStringList> getComponentDataAsStringList() const override
    {
        QList<QStringList> dataLists;
        QStringList data_noise = {
            table_id,
            model,
            brand,
            terminal_shape,
            terminal_size
        };

        // 迭代 noi_out 数组来填充 QStringList
        for (const auto& noi_value : noi) {
            data_noise.append(noi_value);
        }

        data_noise.append(noi_data_source);
        data_noise.append(UUID);

        QStringList data_atten = {
            table_id,
            model,
            brand,
            terminal_shape,
            terminal_size
        };

        // 迭代 noi_out 数组来填充 QStringList
        for (const auto& noi_value : atten) {
            data_atten.append(noi_value);
        }

        data_atten.append(atten_data_source);
        data_atten.append(UUID);

        QStringList data_refl = {
            table_id,
            model,
            brand,
            terminal_shape,
            terminal_size
        };

        // 迭代 noi_out 数组来填充 QStringList
        for (const auto& noi_value : refl) {
            data_refl.push_back(noi_value);
        }

        data_refl.push_back(refl_data_source);
        data_refl.push_back(UUID);

        dataLists.append(data_noise);
        dataLists.append(data_atten);
        dataLists.append(data_refl);

        return dataLists;
    }
}PumpSend;

typedef struct StaticBox_grille : Terminal
{
    QString staticBox_model;    //静压箱型号
    QString staticBox_brand;    //静压箱品牌
    QString grille_model;   //静压箱型号
    QString grille_brand;   //静压箱品牌

    StaticBox_grille() = default;

    StaticBox_grille(const QSqlRecord& record) {
        // Base class member variables (inherited from ComponentBase)
        this->table_id = record.value("table_id").toString();
        this->UUID = record.value("UUID").toString();

        // Terminal specific member variables
        this->terminal_shape = record.value("terminal_shape").toString();
        this->terminal_size = record.value("terminal_size").toString();
        this->noi_data_source = record.value("noi_data_source").toString();
        this->atten_data_source = record.value("atten_data_source").toString();
        this->refl_data_source = record.value("refl_data_source").toString();

        // Handle noise data (assuming `deserializeNoise` function exists)
        QString noiseJson = record.value("noise_json").toString();
        this->noi = deserializeNoise(noiseJson);

        // Handle attenuation data (assuming `deserializeAttenuation` function exists)
        QString attenJson = record.value("atten_json").toString();
        this->atten = deserializeAtten(attenJson);

        // Handle reflection data (assuming `deserializeReflection` function exists)
        QString reflJson = record.value("refl_json").toString();
        this->refl = deserializeAtten(reflJson);

        // StaticBox_grille specific member variables
        this->staticBox_model = record.value("staticBox_model").toString();
        this->staticBox_brand = record.value("staticBox_brand").toString();
        this->grille_model = record.value("grille_model").toString();
        this->grille_brand = record.value("grille_brand").toString();
    }

    StaticBox_grille(const QString& model, const QString& brand, const QString& table_id,
                     const QString& UUID, const QString& noi_data_source,
                     const QString& atten_data_source, const QString& refl_data_source,
                     const QString& shape, const QString& size,
                     const array<QString, 9>& noi, const array<QString, 8>& atten,
                     const array<QString, 8>& refl,
                     const QString& staticBox_model, const QString& staticBox_brand,
                     const QString& grille_model, const QString& grille_brand)
        : Terminal(model, brand, table_id, UUID, noi_data_source, atten_data_source, refl_data_source,
                   shape, size, noi, atten, refl),
          staticBox_model(staticBox_model), staticBox_brand(staticBox_brand),
          grille_model(grille_model), grille_brand(grille_brand)
    {
    }

    // 简化版构造函数，不包含噪音、衰减和反射衰减数组
    StaticBox_grille(const QString& model, const QString& brand, const QString& table_id,
                     const QString& UUID, const QString& noi_data_source,
                     const QString& atten_data_source, const QString& refl_data_source,
                     const QString& shape, const QString& size,
                     const QString& staticBox_model, const QString& staticBox_brand,
                     const QString& grille_model, const QString& grille_brand)
        : Terminal(model, brand, table_id, UUID, noi_data_source, atten_data_source, refl_data_source,
                   shape, size),
          staticBox_model(staticBox_model), staticBox_brand(staticBox_brand),
          grille_model(grille_model), grille_brand(grille_brand)
    {
    }

    // ComponentBase interface
public:
    QString typeName() const override{
        return component_type_name::STATICBOX_GRILLE;
    }

    // ComponentBase interface
public:
    QList<QStringList> getComponentDataAsStringList() const override
    {
        QList<QStringList> dataLists;
        QStringList data_noise = {
            table_id,
            staticBox_model,
            staticBox_brand,
            grille_model,
            grille_brand,
            terminal_shape,
            terminal_size,
        };

        // 迭代 noi_out 数组来填充 QStringList
        for (const auto& noi_value : noi) {
            data_noise.push_back(noi_value);
        }

        data_noise.push_back(noi_data_source);
        data_noise.push_back(UUID);


        QStringList data_atten = {
            table_id,
            staticBox_model,
            staticBox_brand,
            grille_model,
            grille_brand,
            terminal_shape,
            terminal_size,
        };

        // 迭代 noi_out 数组来填充 QStringList
        for (const auto& noi_value : atten) {
            data_atten.push_back(noi_value);
        }

        data_atten.push_back(atten_data_source);
        data_atten.push_back(UUID);

        QStringList data_refl = {
            table_id,
            staticBox_model,
            staticBox_brand,
            grille_model,
            grille_brand,
            terminal_shape,
            terminal_size,
        };

        // 迭代 noi_out 数组来填充 QStringList
        for (const auto& noi_value : refl) {
            data_refl.push_back(noi_value);
        }

        data_refl.push_back(refl_data_source);
        data_refl.push_back(UUID);

        dataLists.append(data_noise);
        dataLists.append(data_atten);
        dataLists.append(data_refl);

        return dataLists;
    }
}StaticBox_grille;

typedef struct Disp_vent_terminal : Terminal
{
    Disp_vent_terminal() = default;

    Disp_vent_terminal(const QSqlRecord& record) {
        // Base class member variables (inherited from ComponentBase)
        this->model = record.value("model").toString();
        this->brand = record.value("brand").toString();
        this->table_id = record.value("table_id").toString();
        this->UUID = record.value("UUID").toString();

        // Terminal specific member variables
        this->terminal_shape = record.value("terminal_shape").toString();
        this->terminal_size = record.value("terminal_size").toString();
        this->noi_data_source = record.value("noi_data_source").toString();
        this->atten_data_source = record.value("atten_data_source").toString();
        this->refl_data_source = record.value("refl_data_source").toString();

        // Handle noise data (assuming `deserializeNoise` function exists)
        QString noiseJson = record.value("noise_json").toString();
        this->noi = deserializeNoise(noiseJson);

        // Handle attenuation data (assuming `deserializeAttenuation` function exists)
        QString attenJson = record.value("atten_json").toString();
        this->atten = deserializeAtten(attenJson);

        // Handle reflection data (assuming `deserializeReflection` function exists)
        QString reflJson = record.value("refl_json").toString();
        this->refl = deserializeAtten(reflJson);
    }

    Disp_vent_terminal(const QString& model, const QString& brand, const QString& table_id,
                       const QString& UUID, const QString& noi_data_source,
                       const QString& atten_data_source, const QString& refl_data_source,
                       const QString& shape, const QString& size,
                       const array<QString, 9>& noi, const array<QString, 8>& atten,
                       const array<QString, 8>& refl)
        : Terminal(model, brand, table_id, UUID, noi_data_source, atten_data_source, refl_data_source,
                   shape, size, noi, atten, refl)
    {
    }

    // 简化版构造函数，不包含噪音、衰减和反射衰减数组
    Disp_vent_terminal(const QString& model, const QString& brand, const QString& table_id,
                       const QString& UUID, const QString& noi_data_source,
                       const QString& atten_data_source, const QString& refl_data_source,
                       const QString& shape, const QString& size)
        : Terminal(model, brand, table_id, UUID, noi_data_source, atten_data_source, refl_data_source,
                   shape, size)
    {
    }

    // ComponentBase interface
public:
    QString typeName() const override{
        return component_type_name::DISP_VENT_TERMINAL;
    }

    // ComponentBase interface
public:
    QList<QStringList> getComponentDataAsStringList() const override
    {
        QList<QStringList> dataLists;
        QStringList data_noise = {
            table_id,
            model,
            brand,
            terminal_shape,
            terminal_size
        };

        // 迭代 noi_out 数组来填充 QStringList
        for (const auto& noi_value : noi) {
            data_noise.push_back(noi_value);
        }

        data_noise.push_back(noi_data_source);
        data_noise.push_back(UUID);


        QStringList data_atten = {
            table_id,
            model,
            brand,
            terminal_shape,
            terminal_size
        };

        // 迭代 noi_out 数组来填充 QStringList
        for (const auto& noi_value : atten) {
            data_atten.push_back(noi_value);
        }

        data_atten.push_back(atten_data_source);
        data_atten.push_back(UUID);

        QStringList data_refl = {
            table_id,
            model,
            brand,
            terminal_shape,
            terminal_size
        };

        // 迭代 noi_out 数组来填充 QStringList
        for (const auto& noi_value : refl) {
            data_refl.push_back(noi_value);
        }

        data_refl.push_back(refl_data_source);
        data_refl.push_back(UUID);

        dataLists.append(data_noise);
        dataLists.append(data_atten);
        dataLists.append(data_refl);

        return dataLists;

    }
}Disp_vent_terminal;

typedef struct Other_send_terminal : Terminal
{
    QString remark; //备注

    Other_send_terminal() = default;

    Other_send_terminal(const QSqlRecord& record) {
        // Base class member variables (inherited from ComponentBase)
        this->model = record.value("model").toString();
        this->brand = record.value("brand").toString();
        this->table_id = record.value("table_id").toString();
        this->UUID = record.value("UUID").toString();

        // Terminal specific member variables
        this->terminal_shape = record.value("terminal_shape").toString();
        this->terminal_size = record.value("terminal_size").toString();
        this->noi_data_source = record.value("noi_data_source").toString();
        this->atten_data_source = record.value("atten_data_source").toString();
        this->refl_data_source = record.value("refl_data_source").toString();

        // Handle noise data (assuming `deserializeNoise` function exists)
        QString noiseJson = record.value("noise_json").toString();
        this->noi = deserializeNoise(noiseJson);

        // Handle attenuation data (assuming `deserializeAttenuation` function exists)
        QString attenJson = record.value("atten_json").toString();
        this->atten = deserializeAtten(attenJson);

        // Handle reflection data (assuming `deserializeReflection` function exists)
        QString reflJson = record.value("refl_json").toString();
        this->refl = deserializeAtten(reflJson);

        // Other_send_terminal specific member variables
        this->remark = record.value("remark").toString();
    }

    Other_send_terminal(const QString& model, const QString& brand, const QString& table_id,
                        const QString& UUID, const QString& noi_data_source,
                        const QString& atten_data_source, const QString& refl_data_source,
                        const QString& shape, const QString& size,
                        const array<QString, 9>& noi, const array<QString, 8>& atten,
                        const array<QString, 8>& refl,
                        const QString& remark)
        : Terminal(model, brand, table_id, UUID, noi_data_source, atten_data_source, refl_data_source,
                   shape, size, noi, atten, refl),
          remark(remark)
    {
    }

    // 简化版构造函数，不包含噪音、衰减和反射衰减数组
    Other_send_terminal(const QString& model, const QString& brand, const QString& table_id,
                        const QString& UUID, const QString& noi_data_source,
                        const QString& atten_data_source, const QString& refl_data_source,
                        const QString& shape, const QString& size,
                        QString remark)
        : Terminal(model, brand, table_id, UUID, noi_data_source, atten_data_source, refl_data_source,
                   shape, size),
          remark(remark)
    {
    }

    // ComponentBase interface
public:
    QString typeName() const override{
        return component_type_name::OTHER_SEND_TERMINAL;
    }

    // ComponentBase interface
public:
    QList<QStringList> getComponentDataAsStringList() const override
    {
        QList<QStringList> dataLists;
        QStringList data_noise = {
            table_id,
            model,
            brand,
            terminal_shape,
            terminal_size
        };

        // 迭代 noi_out 数组来填充 QStringList
        for (const auto& noi_value : noi) {
            data_noise.push_back(noi_value);
        }

        data_noise.push_back(noi_data_source);
        data_noise.push_back(remark);
        data_noise.push_back(UUID);


        QStringList data_atten = {
            table_id,
            model,
            brand,
            terminal_shape,
            terminal_size
        };

        // 迭代 noi_out 数组来填充 QStringList
        for (const auto& noi_value : atten) {
            data_atten.push_back(noi_value);
        }

        data_atten.push_back(atten_data_source);
        data_atten.push_back(remark);
        data_atten.push_back(UUID);

        QStringList data_refl = {
            table_id,
            model,
            brand,
            terminal_shape,
            terminal_size
        };

        // 迭代 noi_out 数组来填充 QStringList
        for (const auto& noi_value : refl) {
            data_refl.push_back(noi_value);
        }

        data_refl.push_back(refl_data_source);
        data_refl.push_back(remark);
        data_refl.push_back(UUID);

        dataLists.append(data_noise);
        dataLists.append(data_atten);
        dataLists.append(data_refl);

        return dataLists;
    }
}Other_send_terminal;

typedef struct Static_box : Branch
{
    // 默认构造函数
    Static_box() = default;

    Static_box(const QSqlRecord& record) {
        // Base class member variables (inherited from ComponentBase)
        this->model = record.value("model").toString();
        this->brand = record.value("brand").toString();
        this->table_id = record.value("table_id").toString();
        this->UUID = record.value("UUID").toString();
        this->data_source = record.value("data_source").toString();

        // Branch specific member variables
        this->q1 = record.value("q1").toDouble(); // Assuming real is stored as double in QSqlRecord
        this->q = record.value("q").toDouble(); // Assuming real is stored as double in QSqlRecord

        // Handle attenuation data (assuming `deserializeAttenuation` function exists)
        QString attenJson = record.value("atten_json").toString();
        this->atten = deserializeAtten(attenJson);
    }

    // 构造函数，包含atten数组参数
    Static_box(const QString& model, const QString& brand, const QString& table_id,
               const QString& UUID, const QString& data_source,
               const QString& q1, const QString& q, const array<QString, 8>& atten)
        : Branch(model, brand, table_id, UUID, data_source, q1, q, atten) {}

    // 构造函数，不包含atten数组参数
    Static_box(const QString& model, const QString& brand, const QString& table_id,
               const QString& UUID, const QString& data_source,
               const QString& q1, const QString& q)
        : Branch(model, brand, table_id, UUID, data_source, q1, q) {}

    // ComponentBase interface
public:
    QString typeName() const override{
        return component_type_name::STATICBOX;
    }

    // ComponentBase interface
public:
    QList<QStringList> getComponentDataAsStringList() const override
    {
        QList<QStringList> dataLists;
        QStringList data = {
            table_id,
            model,
            brand,
            q,
            q1
        };

        // 迭代 noi_out 数组来填充 QStringList
        for (const auto& noi_value : atten) {
            data.push_back(noi_value);
        }

        data.push_back(data_source);
        data.push_back(UUID);

        dataLists.append(data);
        return dataLists;
    }
}Static_box;


typedef struct Multi_ranc : Branch
{
    // 默认构造函数
    Multi_ranc() = default;

    Multi_ranc(const QSqlRecord& record) {
        // Base class member variables (inherited from ComponentBase)
        this->model = record.value("model").toString();
        this->brand = record.value("brand").toString();
        this->table_id = record.value("table_id").toString();
        this->UUID = record.value("UUID").toString();
        this->data_source = record.value("data_source").toString();

        // Branch specific member variables
        this->q1 = record.value("q1").toDouble(); // Assuming real is stored as double in QSqlRecord
        this->q = record.value("q").toDouble(); // Assuming real is stored as double in QSqlRecord

        // Handle attenuation data (assuming `deserializeAttenuation` function exists)
        QString attenJson = record.value("atten_json").toString();
        this->atten = deserializeAtten(attenJson);
    }

    // 构造函数，包含atten数组参数
    Multi_ranc(const QString& model, const QString& brand, const QString& table_id,
               const QString& UUID, const QString& data_source,
               const QString& q1, const QString& q, const array<QString, 8>& atten)
        : Branch(model, brand, table_id, UUID, data_source, q1, q, atten) {}

    // 构造函数，不包含atten数组参数
    Multi_ranc(const QString& model, const QString& brand, const QString& table_id,
               const QString& UUID, const QString& data_source,
               const QString& q1, const QString& q)
        : Branch(model, brand, table_id, UUID, data_source, q1, q) {}

    // ComponentBase interface
public:
    QString typeName() const override{
        return component_type_name::MULTI_RANC;
    }

    // ComponentBase interface
public:
    QList<QStringList> getComponentDataAsStringList() const override
    {
        QList<QStringList> dataLists;
        QStringList data = {
            table_id,
            model,
            brand,
            q,
            q1
        };

        // 迭代 noi_out 数组来填充 QStringList
        for (const auto& noi_value : atten) {
            data.push_back(noi_value);
        }

        data.push_back(data_source);
        data.push_back(UUID);

        dataLists.append(data);
        return dataLists;
    }
}Multi_ranc;

typedef struct Tee : Branch
{
    // 默认构造函数
    Tee() = default;

    Tee(const QSqlRecord& record) {
        // Base class member variables (inherited from ComponentBase)
        this->model = record.value("model").toString();
        this->brand = record.value("brand").toString();
        this->table_id = record.value("table_id").toString();
        this->UUID = record.value("UUID").toString();
        this->data_source = record.value("data_source").toString();

        // Branch specific member variables
        this->q1 = record.value("q1").toDouble(); // Assuming real is stored as double in QSqlRecord
        this->q = record.value("q").toDouble(); // Assuming real is stored as double in QSqlRecord

        // Handle attenuation data (assuming `deserializeAttenuation` function exists)
        QString attenJson = record.value("atten_json").toString();
        this->atten = deserializeAtten(attenJson);
    }

    // 构造函数，包含atten数组参数
    Tee(const QString& model, const QString& brand, const QString& table_id,
        const QString& UUID, const QString& data_source,
        const QString& q1, const QString& q, const array<QString, 8>& atten)
        : Branch(model, brand, table_id, UUID, data_source, q1, q, atten) {}

    // 构造函数，不包含atten数组参数
    Tee(const QString& model, const QString& brand, const QString& table_id,
        const QString& UUID, const QString& data_source,
        const QString& q1, const QString& q)
        : Branch(model, brand, table_id, UUID, data_source, q1, q) {}

    // ComponentBase interface
public:
    QString typeName() const override{
        return component_type_name::TEE;
    }

    // ComponentBase interface
public:
    QList<QStringList> getComponentDataAsStringList() const override
    {
        QList<QStringList> dataLists;
        QStringList data = {
            table_id,
            model,
            brand,
            q,
            q1
        };

        // 迭代 noi_out 数组来填充 QStringList
        for (const auto& noi_value : atten) {
            data.push_back(noi_value);
        }

        data.push_back(data_source);
        data.push_back(UUID);

        dataLists.append(data);
        return dataLists;
    }
}Tee;

typedef struct Pipe : ComponentBase
{

    QString pipe_shape;   //类型
    QString size;      //尺寸
    array<QString, 8> atten;      //送风机进口噪音

    // 默认构造函数
    Pipe() = default;

    Pipe(const QSqlRecord& record) {
        // Base class member variables (inherited from ComponentBase)
        this->model = record.value("model").toString();
        this->brand = record.value("brand").toString();
        this->table_id = record.value("table_id").toString();
        this->UUID = record.value("UUID").toString();
        this->data_source = record.value("data_source").toString();

        // Pipe specific member variables
        this->pipe_shape = record.value("pipe_shape").toString();
        this->size = record.value("size").toString();

        // Handle attenuation data (assuming `deserializeAttenuation` function exists)
        QString attenJson = record.value("atten_json").toString();
        this->atten = deserializeAtten(attenJson);
    }


    // 包含所有成员变量的构造函数
    Pipe(const QString& model, const QString& brand, const QString& table_id,
         const QString& UUID, const QString& data_source,
         const QString& pipe_shape, const QString& size,
         const array<QString, 8>& atten)
        : ComponentBase(model, brand, table_id, data_source, UUID),
          pipe_shape(pipe_shape),
          size(size),
          atten(atten) {}

    // 不包含atten数组的构造函数
    Pipe(const QString& model, const QString& brand, const QString& table_id,
         const QString& UUID, const QString& data_source,
         const QString& pipe_shape, const QString& size)
        : ComponentBase(model, brand, table_id, data_source, UUID),
          pipe_shape(pipe_shape),
          size(size) {}

    // ComponentBase interface
public:
    QString typeName() const override{
        return component_type_name::PIPE;
    }

    // ComponentBase interface
public:
    QList<QStringList> getComponentDataAsStringList() const override
    {
        QList<QStringList> dataLists;
        QStringList data = {
            table_id,
            model,
            brand,
            pipe_shape,
            size
        };

        // 迭代 noi_out 数组来填充 QStringList
        for (const auto& noi_value : atten) {
            data.push_back(noi_value);
        }

        data.push_back(data_source);
        data.push_back(UUID);

        dataLists.append(data);

        return dataLists;
    }
}Pipe;

typedef struct Elbow : ComponentBase
{
    QString elbow_shape;  //类型 圆还是方
    QString size;
    array<QString, 8> atten;

    // 默认构造函数
    Elbow() = default;

    Elbow(const QSqlRecord& record) {
        // Base class member variables (inherited from ComponentBase)
        this->model = record.value("model").toString();
        this->brand = record.value("brand").toString();
        this->table_id = record.value("table_id").toString();
        this->UUID = record.value("UUID").toString();
        this->data_source = record.value("data_source").toString();

        // Elbow specific member variables
        this->elbow_shape = record.value("elbow_shape").toString();
        this->size = record.value("size").toString();

        // Handle attenuation data (assuming `deserializeAttenuation` function exists)
        QString attenJson = record.value("atten_json").toString();
        this->atten = deserializeAtten(attenJson);
    }

    // 包含所有成员变量的构造函数
    Elbow(const QString& model, const QString& brand, const QString& table_id,
          const QString& UUID, const QString& data_source,
          const QString& elbow_shape, const QString& size,
          const array<QString, 8>& atten)
        : ComponentBase(model, brand, table_id, data_source, UUID),
          elbow_shape(elbow_shape),
          size(size),
          atten(atten) {}

    // 不包含noi数组的构造函数
    Elbow(const QString& model, const QString& brand, const QString& table_id,
          const QString& UUID, const QString& data_source,
          const QString& elbow_shape, const QString& size)
        : ComponentBase(model, brand, table_id, data_source, UUID),
          elbow_shape(elbow_shape),
          size(size) {}

    // ComponentBase interface
public:
    QString typeName() const override{
        return component_type_name::ELBOW;
    }

    // ComponentBase interface
public:
    QList<QStringList> getComponentDataAsStringList() const override
    {
        QList<QStringList> dataLists;
        QStringList data = {
            table_id,
            model,
            brand,
            elbow_shape,
            size
        };

        // 迭代 noi_out 数组来填充 QStringList
        for (const auto& noi_value : atten) {
            data.push_back(noi_value);
        }

        data.push_back(data_source);
        data.push_back(UUID);

        dataLists.append(data);
        return dataLists;
    }
}Elbow;

typedef struct Reducer : ComponentBase
{
    QString reducer_type;  //类型 圆还是方
    QString reducer_before_size;
    QString reducer_after_size;
    array<QString, 8> atten;      //送风机进口噪音

    // 默认构造函数
    Reducer() = default;

    Reducer(const QSqlRecord& record) {
        // Base class member variables (inherited from ComponentBase)
        this->model = record.value("model").toString();
        this->brand = record.value("brand").toString();
        this->table_id = record.value("table_id").toString();
        this->UUID = record.value("UUID").toString();
        this->data_source = record.value("data_source").toString();

        // Reducer specific member variables
        this->reducer_type = record.value("reducer_type").toString();
        this->reducer_before_size = record.value("reducer_before_size").toString();
        this->reducer_after_size = record.value("reducer_after_size").toString();

        // Handle attenuation data (assuming `deserializeAttenuation` function exists)
        QString attenJson = record.value("atten_json").toString();
        this->atten = deserializeAtten(attenJson);
    }

    // 包含所有成员变量的构造函数
    Reducer(const QString& model, const QString& brand, const QString& table_id,
            const QString& UUID, const QString& data_source,
            const QString& reducer_type, const QString& reducer_before_size,
            const QString& reducer_after_size, const array<QString, 8>& atten)
        : ComponentBase(model, brand, table_id, data_source, UUID),
          reducer_type(reducer_type),
          reducer_before_size(reducer_before_size),reducer_after_size(reducer_after_size),
          atten(atten) {}

    // 不包含atten数组的构造函数
    Reducer(const QString& model, const QString& brand, const QString& table_id,
            const QString& UUID, const QString& data_source,
            const QString& reducer_type, const QString& reducer_before_size,
            const QString& reducer_after_size)
        : ComponentBase(model, brand, table_id, data_source, UUID),
          reducer_type(reducer_type),
          reducer_before_size(reducer_before_size),reducer_after_size(reducer_after_size) {}

    // ComponentBase interface
public:
    QString typeName() const override{
        return component_type_name::REDUCER;
    }

    // ComponentBase interface
public:
    QList<QStringList> getComponentDataAsStringList() const override
    {
        QList<QStringList> dataLists;
        QStringList data = {
            table_id,
            model,
            brand,
            reducer_type,
            reducer_before_size,
            reducer_after_size
        };

        // 迭代 noi_out 数组来填充 QStringList
        for (const auto& noi_value : atten) {
            data.push_back(noi_value);
        }

        data.push_back(data_source);
        data.push_back(UUID);

        dataLists.append(data);
        return dataLists;
    }
}Reducer;

typedef struct Silencer : ComponentBase
{
    QString silencer_type;       //类型
    array<QString, 8> atten;  // 衰减

    // 默认构造函数
    Silencer() = default;

    Silencer(const QSqlRecord& record) {
        // Base class member variables (inherited from ComponentBase)
        this->model = record.value("model").toString();
        this->brand = record.value("brand").toString();
        this->table_id = record.value("table_id").toString();
        this->UUID = record.value("UUID").toString();
        this->data_source = record.value("data_source").toString();

        // Silencer specific member variable
        this->silencer_type = record.value("silencer_type").toString();

        // Handle attenuation data (assuming `deserializeAttenuation` function exists)
        QString attenJson = record.value("atten_json").toString();
        this->atten = deserializeAtten(attenJson);
    }

    // 包含所有成员变量的构造函数
    Silencer(const QString& model, const QString& brand, const QString& table_id,
             const QString& UUID, const QString& data_source,
             const QString& silencer_type, const array<QString, 8>& atten)
        : ComponentBase(model, brand, table_id, data_source, UUID),
          silencer_type(silencer_type),
          atten(atten) {}

    // 包含所有成员变量的构造函数
    Silencer(const QString& model, const QString& brand, const QString& table_id,
             const QString& UUID, const QString& data_source,
             const QString& silencer_type)
        : ComponentBase(model, brand, table_id, data_source, UUID),
          silencer_type(silencer_type) {}

    // ComponentBase interface
public:
    QString typeName() const override{
        return component_type_name::SILENCER;
    }

    // ComponentBase interface
public:
    QList<QStringList> getComponentDataAsStringList() const override
    {
        QList<QStringList> dataLists;
        QStringList data = {
            table_id,
            model,
            brand
        };

        // 迭代 noi_out 数组来填充 QStringList
        for (const auto& noi_value : atten) {
            data.push_back(noi_value);
        }

        data.push_back(data_source);
        data.push_back(UUID);

        dataLists.append(data);
        return dataLists;
    }
}Silencer;

#endif // COMPONENTSTRUCTS_H
