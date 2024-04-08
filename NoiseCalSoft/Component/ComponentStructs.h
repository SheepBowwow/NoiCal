#ifndef COMPONENTSTRUCTS_H
#define COMPONENTSTRUCTS_H

#include <QString>
#include <QObject>
#include <array>
#include <memory>
#include <QUuid>

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
} Aircondition;

typedef struct VAV_terminal : Valve
{
    QString number;  // 编号

    // 使用Valve的默认构造函数并初始化number
    VAV_terminal() = default;

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
}VAV_terminal;

typedef struct Circular_damper : Valve
{
    // 使用Valve的默认构造函数
    QString diameter;

    Circular_damper() : Valve() {}

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
}Circular_damper;

typedef struct Rect_damper : Valve
{
    // 使用Valve的默认构造函数

    QString length;
    QString width;
    QString size;

    Rect_damper() : Valve() {}

    // 使用Valve的全部参数构造函数初始化基类部分
    Rect_damper(const QString& model, const QString& brand, const QString& table_id,
                const QString& UUID, const QString& data_source,
                const QString& angle, const QString& air_volume,
                const array<QString, 9>& noi, const QString& length,  const QString& width)
        : Valve(model, brand, table_id, data_source, UUID, angle, air_volume, noi),
          length(length), width(width) {}

    // 使用Valve的不包含噪音数组的构造函数初始化基类部分
    Rect_damper(const QString& model, const QString& brand, const QString& table_id,
                const QString& UUID, const QString& data_source,
                const QString& angle, const QString& air_volume,
                const QString& length,  const QString& width )
        : Valve(model, brand, table_id, data_source, UUID, angle, air_volume),
          length(length), width(width) {}

    // ComponentBase interface
public:
    QString typeName() const override{
        return component_type_name::RECT_DAMPER;
    }
}Rect_damper;

typedef struct AirDiff : Terminal
{
    QString air_distributor_model;  // 布风器型号
    QString air_distributor_brand;  // 品牌
    QString diffuser_model;  // 散流器型号
    QString diffuser_brand;  // 品牌

    AirDiff() = default;

    AirDiff(const QString& model, const QString& brand, const QString& table_id,
            const QString& UUID, const QString& noi_data_source,
            const QString& atten_data_source, const QString& refl_data_source,
            const QString& shape, const QString& terminal_size,
            const array<QString, 9>& noi, const array<QString, 8>& atten,
            const array<QString, 8>& refl,
            const QString& air_distributor_model, const QString& air_distributor_brand,
            const QString& diffuser_model, const QString& diffuser_brand)
        : Terminal(model, brand, table_id, noi_data_source, atten_data_source, refl_data_source, UUID,
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
        : Terminal(model, brand, table_id, noi_data_source, atten_data_source, refl_data_source, UUID,
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
}AirDiff;

typedef struct PumpSend : Terminal
{
    QString type_pump_or_send;  //类型 抽风头or送风头

    PumpSend() = default;

    PumpSend(const QString& model, const QString& brand, const QString& table_id,
             const QString& UUID, const QString& noi_data_source,
             const QString& atten_data_source, const QString& refl_data_source,
             const QString& shape, const QString& size,
             const array<QString, 9>& noi, const array<QString, 8>& atten,
             const array<QString, 8>& refl,
             const QString& type)
        : Terminal(model, brand, table_id, noi_data_source, atten_data_source, refl_data_source, UUID,
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
        : Terminal(model, brand, table_id, noi_data_source, atten_data_source, refl_data_source, UUID,
                   shape, size),
          type_pump_or_send(type)
    {
    }

    // ComponentBase interface
public:
    QString typeName() const override{
        return component_type_name::PUMPSEND;
    }
}PumpSend;

typedef struct StaticBox_grille : Terminal
{
    QString staticBox_model;    //静压箱型号
    QString staticBox_brand;    //静压箱品牌
    QString grille_model;   //静压箱型号
    QString grille_brand;   //静压箱品牌

    StaticBox_grille() = default;

    StaticBox_grille(const QString& model, const QString& brand, const QString& table_id,
                     const QString& UUID, const QString& noi_data_source,
                     const QString& atten_data_source, const QString& refl_data_source,
                     const QString& shape, const QString& size,
                     const array<QString, 9>& noi, const array<QString, 8>& atten,
                     const array<QString, 8>& refl,
                     const QString& staticBox_model, const QString& staticBox_brand,
                     const QString& grille_model, const QString& grille_brand)
        : Terminal(model, brand, table_id, noi_data_source, atten_data_source, refl_data_source, UUID,
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
        : Terminal(model, brand, table_id, noi_data_source, atten_data_source, refl_data_source, UUID,
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
}StaticBox_grille;

typedef struct Disp_vent_terminal : Terminal
{
    Disp_vent_terminal() = default;

    Disp_vent_terminal(const QString& model, const QString& brand, const QString& table_id,
                       const QString& UUID, const QString& noi_data_source,
                       const QString& atten_data_source, const QString& refl_data_source,
                       const QString& shape, const QString& size,
                       const array<QString, 9>& noi, const array<QString, 8>& atten,
                       const array<QString, 8>& refl)
        : Terminal(model, brand, table_id, noi_data_source, atten_data_source, refl_data_source, UUID,
                   shape, size, noi, atten, refl)
    {
    }

    // 简化版构造函数，不包含噪音、衰减和反射衰减数组
    Disp_vent_terminal(const QString& model, const QString& brand, const QString& table_id,
                       const QString& UUID, const QString& noi_data_source,
                       const QString& atten_data_source, const QString& refl_data_source,
                       const QString& shape, const QString& size)
        : Terminal(model, brand, table_id, noi_data_source, atten_data_source, refl_data_source, UUID,
                   shape, size)
    {
    }

    // ComponentBase interface
public:
    QString typeName() const override{
        return component_type_name::DISP_VENT_TERMINAL;
    }
}Disp_vent_terminal;

typedef struct Other_send_terminal : Terminal
{
    QString remark; //备注

    Other_send_terminal() = default;

    Other_send_terminal(const QString& model, const QString& brand, const QString& table_id,
                        const QString& UUID, const QString& noi_data_source,
                        const QString& atten_data_source, const QString& refl_data_source,
                        const QString& shape, const QString& size,
                        const array<QString, 9>& noi, const array<QString, 8>& atten,
                        const array<QString, 8>& refl,
                        const QString& remark)
        : Terminal(model, brand, table_id, noi_data_source, atten_data_source, refl_data_source, UUID,
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
        : Terminal(model, brand, table_id, noi_data_source, atten_data_source, refl_data_source, UUID,
                   shape, size),
          remark(remark)
    {
    }

    // ComponentBase interface
public:
    QString typeName() const override{
        return component_type_name::OTHER_SEND_TERMINAL;
    }
}Other_send_terminal;

typedef struct Static_box : Branch
{
    // 默认构造函数
    Static_box() = default;

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
}Static_box;


typedef struct Multi_ranc : Branch
{
    // 默认构造函数
    Multi_ranc() = default;

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
}Multi_ranc;

typedef struct Tee : Branch
{
    // 默认构造函数
    Tee() = default;

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
}Tee;

typedef struct Pipe : ComponentBase
{

    QString pipe_shape;   //类型
    QString size;      //尺寸
    array<QString, 8> atten;      //送风机进口噪音

    // 默认构造函数
    Pipe() = default;

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
}Pipe;

typedef struct Elbow : ComponentBase
{
    QString elbow_shape;  //类型 圆还是方
    QString size;
    array<QString, 8> atten;

    // 默认构造函数
    Elbow() = default;

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
}Elbow;

typedef struct Reducer : ComponentBase
{
    QString reducer_type;  //类型 圆还是方
    QString reducer_before_size;
    QString reducer_after_size;
    array<QString, 8> atten;      //送风机进口噪音

    // 默认构造函数
    Reducer() = default;

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
}Reducer;

typedef struct Silencer : ComponentBase
{
    QString silencer_type;       //类型
    array<QString, 8> atten;  // 衰减

    // 默认构造函数
    Silencer() = default;

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
}Silencer;

#endif // COMPONENTSTRUCTS_H
