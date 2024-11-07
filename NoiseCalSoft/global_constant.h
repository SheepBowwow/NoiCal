#ifndef GLOBAL_CONSTANT_H
#define GLOBAL_CONSTANT_H

#define AIR_DIFF_MODEL "air_diffuser_model"
#define AIR_DIFF_TABLE "air_diffuser_noi"

#define PUMP_SEND_TUYERE_MODEL "model"
#define PUMP_SEND_TUYERE_TABLE "pump_send_tuyere"

#define RETURN_AIR_BOX_GRILLE_MODEL "return_air_box_grille_model"
#define RETURN_AIR_BOX_GRILLE_TABLE "return_air_box_and_grille_noi"

#define SILENCER_MODEL "silencer_model"
#define SILENCER_TYPE "silencer_type"
#define SILENCER_TABLE "silencer_atten"

#define TERMINAL_MODEL "terminal_model"
#define TERMINAL_TABLE "terminal_atten"

#define REPORT_TEMPLATE_FILEPATH "document\\template.docx"

#include <QString>
#include <QMap>

// 定义枚举类型
enum class ComponentType {
    FAN = 101,
    FANCOIL,
    AIRCONDITION,
    AIRCONDITION_SINGLE,
    AIRCONDITION_DOUBLE,
    VAV_TERMINAL,
    CIRCULAR_DAMPER,
    RECT_DAMPER,
    AIRDIFF,
    PUMPSEND,
    PUMP,
    SEND,
    STATICBOX_GRILLE,
    DISP_VENT_TERMINAL,
    OTHER_SEND_TERMINAL,
    STATICBOX,
    MULTI_RANC,
    DIFFUSER_BRANCH,
    TEE,
    PIPE,
    ELBOW,
    REDUCER,
    SILENCER,
    SILENCER_CIRCLE,
    SILENCER_RECT,
    SILENCER_CIRCLE_ELBOW,
    SILENCER_RECT_ELBOW,
    UNDEFINED
};

enum class RoomCalTableType {
    ROOM_LESS425 = 1001,
    ROOM_MORE425,
    ROOM_NOFURNITURE,
    ROOM_OPEN,
    ROOM_GAP_TUYERE,
    ROOM_RAIN,
    FAN = static_cast<int>(ComponentType::FAN),
    FANCOIL = static_cast<int>(ComponentType::FANCOIL),
    AIRCONDITION = static_cast<int>(ComponentType::AIRCONDITION),
    AIRCONDITION_SINGLE = static_cast<int>(ComponentType::AIRCONDITION_SINGLE),
    AIRCONDITION_DOUBLE = static_cast<int>(ComponentType::AIRCONDITION_DOUBLE),
    VAV_TERMINAL = static_cast<int>(ComponentType::VAV_TERMINAL),
    CIRCULAR_DAMPER = static_cast<int>(ComponentType::CIRCULAR_DAMPER),
    RECT_DAMPER = static_cast<int>(ComponentType::RECT_DAMPER),
    AIRDIFF = static_cast<int>(ComponentType::AIRDIFF),
    PUMPSEND = static_cast<int>(ComponentType::PUMPSEND),
    PUMP = static_cast<int>(ComponentType::PUMP),
    SEND = static_cast<int>(ComponentType::SEND),
    STATICBOX_GRILLE = static_cast<int>(ComponentType::STATICBOX_GRILLE),
    DISP_VENT_TERMINAL = static_cast<int>(ComponentType::DISP_VENT_TERMINAL),
    OTHER_SEND_TERMINAL = static_cast<int>(ComponentType::OTHER_SEND_TERMINAL),
    STATICBOX = static_cast<int>(ComponentType::STATICBOX),
    MULTI_RANC = static_cast<int>(ComponentType::MULTI_RANC),
    DIFFUSER_BRANCH = static_cast<int>(ComponentType::DIFFUSER_BRANCH),
    TEE = static_cast<int>(ComponentType::TEE),
    PIPE = static_cast<int>(ComponentType::PIPE),
    ELBOW = static_cast<int>(ComponentType::ELBOW),
    REDUCER = static_cast<int>(ComponentType::REDUCER),
    SILENCER = static_cast<int>(ComponentType::SILENCER),
    SILENCER_CIRCLE = static_cast<int>(ComponentType::SILENCER_CIRCLE),
    SILENCER_RECT = static_cast<int>(ComponentType::SILENCER_RECT),
    SILENCER_CIRCLE_ELBOW = static_cast<int>(ComponentType::SILENCER_CIRCLE_ELBOW),
    SILENCER_RECT_ELBOW = static_cast<int>(ComponentType::SILENCER_RECT_ELBOW),
    UNDEFINED = static_cast<int>(ComponentType::UNDEFINED)
};

// 创建一个函数来将枚举值转换为字符串
QString componentTypeToString(ComponentType type);

ComponentType stringToComponentType(const QString& str);

QString roomCalTableTypeToString(RoomCalTableType type);

RoomCalTableType stringToRoomCalTableType(const QString &str);

#endif // GLOBAL_CONSTANT_H
