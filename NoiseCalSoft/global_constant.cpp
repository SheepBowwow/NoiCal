#include "global_constant.h"

QString componentTypeToString(ComponentType type) {
    switch (type) {
    case ComponentType::FAN:
        return QStringLiteral("风机");
    case ComponentType::FANCOIL:
        return QStringLiteral("风机盘管");
    case ComponentType::AIRCONDITION:
        return QStringLiteral("空调器");
    case ComponentType::AIRCONDITION_SINGLE:
        return QStringLiteral("空调器(单风机)");
    case ComponentType::AIRCONDITION_DOUBLE:
        return QStringLiteral("空调器(双风机)");
    case ComponentType::VAV_TERMINAL:
        return QStringLiteral("变风量末端");
    case ComponentType::CIRCULAR_DAMPER:
        return QStringLiteral("圆形调风门");
    case ComponentType::RECT_DAMPER:
        return QStringLiteral("方形调风门");
    case ComponentType::AIRDIFF:
        return QStringLiteral("布风器+散流器");
    case ComponentType::PUMPSEND:
        return QStringLiteral("抽/送风头");
    case ComponentType::PUMP:
        return QStringLiteral("抽风头");
    case ComponentType::SEND:
        return QStringLiteral("送风头");
    case ComponentType::STATICBOX_GRILLE:
        return QStringLiteral("静压箱+格栅");
    case ComponentType::DISP_VENT_TERMINAL:
        return QStringLiteral("置换通风末端");
    case ComponentType::OTHER_SEND_TERMINAL:
        return QStringLiteral("其他送风末端");
    case ComponentType::STATICBOX:
        return QStringLiteral("静压箱");
    case ComponentType::MULTI_RANC:
        return QStringLiteral("风道多分支");
    case ComponentType::TEE:
        return QStringLiteral("三通");
    case ComponentType::PIPE:
        return QStringLiteral("直管");
    case ComponentType::ELBOW:
        return QStringLiteral("弯头");
    case ComponentType::REDUCER:
        return QStringLiteral("变径");
    case ComponentType::SILENCER:
        return QStringLiteral("消音器");
    case ComponentType::SILENCER_CIRCLE:
        return QStringLiteral("圆形消音器");
    case ComponentType::SILENCER_RECT:
        return QStringLiteral("方形消音器");
    case ComponentType::SILENCER_CIRCLE_ELBOW:
        return QStringLiteral("圆形消音弯头");
    case ComponentType::SILENCER_RECT_ELBOW:
        return QStringLiteral("方形消音弯头");
    default:
        return QStringLiteral("未知类型");
    }
}



ComponentType stringToComponentType(const QString &str) {
    static const QMap<QString, ComponentType> stringToTypeMap =
        {
         {QStringLiteral("风机"), ComponentType::FAN},
         {QStringLiteral("风机盘管"), ComponentType::FANCOIL},
         {QStringLiteral("空调器"), ComponentType::AIRCONDITION},
         {QStringLiteral("空调器(单风机)"), ComponentType::AIRCONDITION_SINGLE},
         {QStringLiteral("空调器(双风机)"), ComponentType::AIRCONDITION_DOUBLE},
         {QStringLiteral("变风量末端"), ComponentType::VAV_TERMINAL},
         {QStringLiteral("圆形调风门"), ComponentType::CIRCULAR_DAMPER},
         {QStringLiteral("方形调风门"), ComponentType::RECT_DAMPER},
         {QStringLiteral("布风器+散流器"), ComponentType::AIRDIFF},
         {QStringLiteral("抽/送风头"), ComponentType::PUMPSEND},
         {QStringLiteral("抽风头"), ComponentType::PUMP},
         {QStringLiteral("送风头"), ComponentType::SEND},
         {QStringLiteral("静压箱+格栅"), ComponentType::STATICBOX_GRILLE},
         {QStringLiteral("置换通风末端"), ComponentType::DISP_VENT_TERMINAL},
         {QStringLiteral("其他送风末端"), ComponentType::OTHER_SEND_TERMINAL},
         {QStringLiteral("静压箱"), ComponentType::STATICBOX},
         {QStringLiteral("风道多分支"), ComponentType::MULTI_RANC},
         {QStringLiteral("三通"), ComponentType::TEE},
         {QStringLiteral("直管"), ComponentType::PIPE},
         {QStringLiteral("弯头"), ComponentType::ELBOW},
         {QStringLiteral("变径"), ComponentType::REDUCER},
         {QStringLiteral("消音器"), ComponentType::SILENCER},
         {QStringLiteral("圆形消音器"), ComponentType::SILENCER_CIRCLE},
         {QStringLiteral("方形消音器"), ComponentType::SILENCER_RECT},
         {QStringLiteral("圆形消音弯头"), ComponentType::SILENCER_CIRCLE_ELBOW},
         {QStringLiteral("方形消音弯头"), ComponentType::SILENCER_RECT_ELBOW},
         };

    return stringToTypeMap.value(str, ComponentType::UNDEFINED);
}

QString roomCalTableTypeToString(RoomCalTableType type) {
    switch (type) {
    case RoomCalTableType::ROOM_LESS425:
        return QStringLiteral("类型1");
    case RoomCalTableType::ROOM_MORE425:
        return QStringLiteral("类型2");
    case RoomCalTableType::ROOM_NOFURNITURE:
        return QStringLiteral("类型3");
    case RoomCalTableType::ROOM_OPEN:
        return QStringLiteral("类型4");
    case RoomCalTableType::ROOM_GAP_TUYERE:
        return QStringLiteral("类型5");
    case RoomCalTableType::ROOM_RAIN:
        return QStringLiteral("类型6");
    case RoomCalTableType::FAN:
        return QStringLiteral("风机");
    case RoomCalTableType::FANCOIL:
        return QStringLiteral("风机盘管");
    case RoomCalTableType::AIRCONDITION:
        return QStringLiteral("空调器");
    case RoomCalTableType::AIRCONDITION_SINGLE:
        return QStringLiteral("空调器(单风机)");
    case RoomCalTableType::AIRCONDITION_DOUBLE:
        return QStringLiteral("空调器(双风机)");
    case RoomCalTableType::VAV_TERMINAL:
        return QStringLiteral("变风量末端");
    case RoomCalTableType::CIRCULAR_DAMPER:
        return QStringLiteral("圆形调风门");
    case RoomCalTableType::RECT_DAMPER:
        return QStringLiteral("方形调风门");
    case RoomCalTableType::AIRDIFF:
        return QStringLiteral("布风器+散流器");
    case RoomCalTableType::PUMPSEND:
        return QStringLiteral("抽/送风头");
    case RoomCalTableType::PUMP:
        return QStringLiteral("抽风头");
    case RoomCalTableType::SEND:
        return QStringLiteral("送风头");
    case RoomCalTableType::STATICBOX_GRILLE:
        return QStringLiteral("静压箱+格栅");
    case RoomCalTableType::DISP_VENT_TERMINAL:
        return QStringLiteral("置换通风末端");
    case RoomCalTableType::OTHER_SEND_TERMINAL:
        return QStringLiteral("其他送风末端");
    case RoomCalTableType::STATICBOX:
        return QStringLiteral("静压箱");
    case RoomCalTableType::MULTI_RANC:
        return QStringLiteral("风道多分支");
    case RoomCalTableType::TEE:
        return QStringLiteral("三通");
    case RoomCalTableType::PIPE:
        return QStringLiteral("直管");
    case RoomCalTableType::ELBOW:
        return QStringLiteral("弯头");
    case RoomCalTableType::REDUCER:
        return QStringLiteral("变径");
    case RoomCalTableType::SILENCER:
        return QStringLiteral("消音器");
    case RoomCalTableType::SILENCER_CIRCLE:
        return QStringLiteral("圆形消音器");
    case RoomCalTableType::SILENCER_RECT:
        return QStringLiteral("方形消音器");
    case RoomCalTableType::SILENCER_CIRCLE_ELBOW:
        return QStringLiteral("圆形消音弯头");
    case RoomCalTableType::SILENCER_RECT_ELBOW:
        return QStringLiteral("方形消音弯头");
    default:
        return QStringLiteral("未知类型");
    }
}

RoomCalTableType stringToRoomCalTableType(const QString &str) {
    static const QMap<QString, RoomCalTableType> stringToRoomTypeMap =
        {
            {QStringLiteral("类型1"), RoomCalTableType::ROOM_LESS425},
            {QStringLiteral("类型2"), RoomCalTableType::ROOM_MORE425},
            {QStringLiteral("类型3"), RoomCalTableType::ROOM_NOFURNITURE},
            {QStringLiteral("类型4"), RoomCalTableType::ROOM_OPEN},
            {QStringLiteral("类型5"), RoomCalTableType::ROOM_GAP_TUYERE},
            {QStringLiteral("类型6"), RoomCalTableType::ROOM_RAIN},
            {QStringLiteral("风机"), RoomCalTableType::FAN},
            {QStringLiteral("风机盘管"), RoomCalTableType::FANCOIL},
            {QStringLiteral("空调器"), RoomCalTableType::AIRCONDITION},
            {QStringLiteral("空调器(单风机)"), RoomCalTableType::AIRCONDITION_SINGLE},
            {QStringLiteral("空调器(双风机)"), RoomCalTableType::AIRCONDITION_DOUBLE},
            {QStringLiteral("变风量末端"), RoomCalTableType::VAV_TERMINAL},
            {QStringLiteral("圆形调风门"), RoomCalTableType::CIRCULAR_DAMPER},
            {QStringLiteral("方形调风门"), RoomCalTableType::RECT_DAMPER},
            {QStringLiteral("布风器+散流器"), RoomCalTableType::AIRDIFF},
            {QStringLiteral("抽/送风头"), RoomCalTableType::PUMPSEND},
            {QStringLiteral("抽风头"), RoomCalTableType::PUMP},
            {QStringLiteral("送风头"), RoomCalTableType::SEND},
            {QStringLiteral("静压箱+格栅"), RoomCalTableType::STATICBOX_GRILLE},
            {QStringLiteral("置换通风末端"), RoomCalTableType::DISP_VENT_TERMINAL},
            {QStringLiteral("其他送风末端"), RoomCalTableType::OTHER_SEND_TERMINAL},
            {QStringLiteral("静压箱"), RoomCalTableType::STATICBOX},
            {QStringLiteral("风道多分支"), RoomCalTableType::MULTI_RANC},
            {QStringLiteral("三通"), RoomCalTableType::TEE},
            {QStringLiteral("直管"), RoomCalTableType::PIPE},
            {QStringLiteral("弯头"), RoomCalTableType::ELBOW},
            {QStringLiteral("变径"), RoomCalTableType::REDUCER},
            {QStringLiteral("消音器"), RoomCalTableType::SILENCER},
            {QStringLiteral("圆形消音器"), RoomCalTableType::SILENCER_CIRCLE},
            {QStringLiteral("方形消音器"), RoomCalTableType::SILENCER_RECT},
            {QStringLiteral("圆形消音弯头"), RoomCalTableType::SILENCER_CIRCLE_ELBOW},
            {QStringLiteral("方形消音弯头"), RoomCalTableType::SILENCER_RECT_ELBOW},
        };

    return stringToRoomTypeMap.value(str, RoomCalTableType::UNDEFINED);  // 默认值为 UNDEFINED
}
