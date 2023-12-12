QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# 禁用-Wunknown-pragmas报警
CONFIG += warn_off
QMAKE_CXXFLAGS += -Wall
QMAKE_CXXFLAGS += -Wno-unknown-pragmas

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Component/ComponentManager.cpp \
    create_prj.cpp \
    databasemanager.cpp \
    globle_var.cpp \
    inputDialog/dialog_air_diff.cpp \
    inputDialog/dialog_aircondition_noise.cpp \
    inputDialog/dialog_circular_damper.cpp \
    inputDialog/dialog_disp_vent_terminal.cpp \
    inputDialog/dialog_duct_with_multi_ranc.cpp \
    inputDialog/dialog_elbow.cpp \
    inputDialog/dialog_fan_noise.cpp \
    inputDialog/dialog_fancoil_noise.cpp \
    inputDialog/dialog_other_send_terminal.cpp \
    inputDialog/dialog_pipe.cpp \
    inputDialog/dialog_pump_send.cpp \
    inputDialog/dialog_rect_damper.cpp \
    inputDialog/dialog_reducer.cpp \
    inputDialog/dialog_returnairbox_grille.cpp \
    inputDialog/dialog_silencer.cpp \
    inputDialog/dialog_static_box.cpp \
    inputDialog/dialog_tee.cpp \
    inputDialog/dialog_vav_terminal.cpp \
    inputbasedialog.cpp \
    main.cpp \
    roomCal/room_cal_basewidget.cpp \
    roomCal/roomcaltable.cpp \
    roomDefineForm/dialog_add_zhushuqu.cpp \
    roomDefineForm/dialog_addroom.cpp \
    roomDefineForm/form_room_define.cpp \
    widget.cpp

HEADERS += \
    Component/ComponentManager.h \
    Component/ComponentStructs.h \
    create_prj.h \
    databasemanager.h \
    global_constant.h \
    globle_var.h \
    inputDialog/dialog_air_diff.h \
    inputDialog/dialog_aircondition_noise.h \
    inputDialog/dialog_circular_damper.h \
    inputDialog/dialog_disp_vent_terminal.h \
    inputDialog/dialog_duct_with_multi_ranc.h \
    inputDialog/dialog_elbow.h \
    inputDialog/dialog_fan_noise.h \
    inputDialog/dialog_fancoil_noise.h \
    inputDialog/dialog_other_send_terminal.h \
    inputDialog/dialog_pipe.h \
    inputDialog/dialog_pump_send.h \
    inputDialog/dialog_rect_damper.h \
    inputDialog/dialog_reducer.h \
    inputDialog/dialog_returnairbox_grille.h \
    inputDialog/dialog_silencer.h \
    inputDialog/dialog_static_box.h \
    inputDialog/dialog_tee.h \
    inputDialog/dialog_vav_terminal.h \
    inputbasedialog.h \
    roomCal/room_cal_basewidget.h \
    roomCal/roomcaltable.h \
    roomDefineForm/dialog_add_zhushuqu.h \
    roomDefineForm/dialog_addroom.h \
    roomDefineForm/form_room_define.h \
    widget.h

FORMS += \
    create_prj.ui \
    inputDialog/dialog_air_diff.ui \
    inputDialog/dialog_aircondition_noise.ui \
    inputDialog/dialog_circular_damper.ui \
    inputDialog/dialog_disp_vent_terminal.ui \
    inputDialog/dialog_duct_with_multi_ranc.ui \
    inputDialog/dialog_elbow.ui \
    inputDialog/dialog_fan_noise.ui \
    inputDialog/dialog_fancoil_noise.ui \
    inputDialog/dialog_other_send_terminal.ui \
    inputDialog/dialog_pipe.ui \
    inputDialog/dialog_pump_send.ui \
    inputDialog/dialog_rect_damper.ui \
    inputDialog/dialog_reducer.ui \
    inputDialog/dialog_returnairbox_grille.ui \
    inputDialog/dialog_silencer.ui \
    inputDialog/dialog_static_box.ui \
    inputDialog/dialog_tee.ui \
    inputDialog/dialog_vav_terminal.ui \
    roomCal/room_cal_basewidget.ui \
    roomCal/roomcaltable.ui \
    roomDefineForm/dialog_add_zhushuqu.ui \
    roomDefineForm/dialog_addroom.ui \
    roomDefineForm/form_room_define.ui \
    widget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    src.qrc
