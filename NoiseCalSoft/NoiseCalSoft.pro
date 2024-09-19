QT       += core gui sql
QT       += axcontainer

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# 禁用-Wunknown-pragmas报警
CONFIG += warn_off
QMAKE_CXXFLAGS += -Wall
QMAKE_CXXFLAGS += -Wno-unknown-pragmas

DESTDIR = ./bin

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Component/ComponentManager.cpp \
    Component/componentsdb.cpp \
    calFunction/cal_function.cpp \
    componentInpuTableWidget/widget_air_diff_inputtable.cpp \
    componentInpuTableWidget/widget_aircondition_doublefan_inputtable.cpp \
    componentInpuTableWidget/widget_aircondition_singlefan_inputtable.cpp \
    componentInpuTableWidget/widget_base_inputtable.cpp \
    componentInpuTableWidget/widget_circular_damper_inputtable.cpp \
    componentInpuTableWidget/widget_disp_vent_terminal_inputtable.cpp \
    componentInpuTableWidget/widget_elbow_inputtable.cpp \
    componentInpuTableWidget/widget_fan_inputtable.cpp \
    componentInpuTableWidget/widget_fancoil_inputtable.cpp \
    componentInpuTableWidget/widget_multi_ranc_inputtable.cpp \
    componentInpuTableWidget/widget_other_send_terminal_inputtable.cpp \
    componentInpuTableWidget/widget_pipe_inputtable.cpp \
    componentInpuTableWidget/widget_pump_send_inputtable.cpp \
    componentInpuTableWidget/widget_rect_damper_inputtable.cpp \
    componentInpuTableWidget/widget_reducer_inputtable.cpp \
    componentInpuTableWidget/widget_silencer_inputtable.cpp \
    componentInpuTableWidget/widget_static_box_grille_inputtable.cpp \
    componentInpuTableWidget/widget_static_box_inputtable.cpp \
    componentInpuTableWidget/widget_tee_inputtable.cpp \
    componentInpuTableWidget/widget_vav_terminal_inputtable.cpp \
    database/DatabaseOperations.cpp \
    databasemanager.cpp \
    global_constant.cpp \
    globle_var.cpp \
    inputDialog/dialog_air_diff.cpp \
    inputDialog/dialog_aircondition.cpp \
    inputDialog/dialog_circular_damper.cpp \
    inputDialog/dialog_disp_vent_terminal.cpp \
    inputDialog/dialog_duct_with_multi_ranc.cpp \
    inputDialog/dialog_elbow.cpp \
    inputDialog/dialog_fan.cpp \
    inputDialog/dialog_fancoil.cpp \
    inputDialog/dialog_other_send_terminal.cpp \
    inputDialog/dialog_pipe.cpp \
    inputDialog/dialog_prj_manager.cpp \
    inputDialog/dialog_pump_send.cpp \
    inputDialog/dialog_rect_damper.cpp \
    inputDialog/dialog_reducer.cpp \
    inputDialog/dialog_silencer.cpp \
    inputDialog/dialog_staticBox_grille.cpp \
    inputDialog/dialog_static_box.cpp \
    inputDialog/dialog_tee.cpp \
    inputDialog/dialog_vav_terminal.cpp \
    inputbasedialog.cpp \
    main.cpp \
    project/projectmanager.cpp \
    roomCal/outer_after_widget.cpp \
    roomCal/outer_before_widget.cpp \
    roomCal/room_cal_basewidget.cpp \
    roomCal/room_cal_total_widget.cpp \
    roomCal/roomcaltable.cpp \
    roomDefineForm/dialog_add_room.cpp \
    roomDefineForm/dialog_add_system.cpp \
    roomDefineForm/dialog_set_name.cpp \
    roomDefineForm/room_define_widget.cpp \
    roomDefineForm/roomcalinfomanager.cpp \
    roomDefineForm/systemcomp_list_widget.cpp \
    subclass/clickablecombobox.cpp \
    widget.cpp \
    wordEngine/wordengine.cpp

HEADERS += \
    Component/ComponentManager.h \
    Component/ComponentStructs.h \
    Component/componentsdb.h \
    calFunction/cal_function.h \
    componentInpuTableWidget/widget_air_diff_inputtable.h \
    componentInpuTableWidget/widget_aircondition_doublefan_inputtable.h \
    componentInpuTableWidget/widget_aircondition_singlefan_inputtable.h \
    componentInpuTableWidget/widget_base_inputtable.h \
    componentInpuTableWidget/widget_circular_damper_inputtable.h \
    componentInpuTableWidget/widget_disp_vent_terminal_inputtable.h \
    componentInpuTableWidget/widget_elbow_inputtable.h \
    componentInpuTableWidget/widget_fan_inputtable.h \
    componentInpuTableWidget/widget_fancoil_inputtable.h \
    componentInpuTableWidget/widget_multi_ranc_inputtable.h \
    componentInpuTableWidget/widget_other_send_terminal_inputtable.h \
    componentInpuTableWidget/widget_pipe_inputtable.h \
    componentInpuTableWidget/widget_pump_send_inputtable.h \
    componentInpuTableWidget/widget_rect_damper_inputtable.h \
    componentInpuTableWidget/widget_reducer_inputtable.h \
    componentInpuTableWidget/widget_silencer_inputtable.h \
    componentInpuTableWidget/widget_static_box_grille_inputtable.h \
    componentInpuTableWidget/widget_static_box_inputtable.h \
    componentInpuTableWidget/widget_tee_inputtable.h \
    componentInpuTableWidget/widget_vav_terminal_inputtable.h \
    database/DatabaseOperations.h \
    databasemanager.h \
    global_constant.h \
    globle_var.h \
    inputDialog/dialog_air_diff.h \
    inputDialog/dialog_aircondition.h \
    inputDialog/dialog_circular_damper.h \
    inputDialog/dialog_disp_vent_terminal.h \
    inputDialog/dialog_duct_with_multi_ranc.h \
    inputDialog/dialog_elbow.h \
    inputDialog/dialog_fan.h \
    inputDialog/dialog_fancoil.h \
    inputDialog/dialog_other_send_terminal.h \
    inputDialog/dialog_pipe.h \
    inputDialog/dialog_prj_manager.h \
    inputDialog/dialog_pump_send.h \
    inputDialog/dialog_rect_damper.h \
    inputDialog/dialog_reducer.h \
    inputDialog/dialog_silencer.h \
    inputDialog/dialog_staticBox_grille.h \
    inputDialog/dialog_static_box.h \
    inputDialog/dialog_tee.h \
    inputDialog/dialog_vav_terminal.h \
    inputbasedialog.h \
    project/projectmanager.h \
    roomCal/outer_after_widget.h \
    roomCal/outer_before_widget.h \
    roomCal/room_cal_basewidget.h \
    roomCal/room_cal_total_widget.h \
    roomCal/roomcaltable.h \
    roomDefineForm/dialog_add_room.h \
    roomDefineForm/dialog_add_system.h \
    roomDefineForm/dialog_set_name.h \
    roomDefineForm/room_define_widget.h \
    roomDefineForm/roomcalinfomanager.h \
    roomDefineForm/systemcomp_list_widget.h \
    subclass/clickablecombobox.h \
    widget.h \
    wordEngine/wordengine.h

FORMS += \
    Component/componentsdb.ui \
    componentInpuTableWidget/widget_base_inputtable.ui \
    inputDialog/dialog_air_diff.ui \
    inputDialog/dialog_aircondition.ui \
    inputDialog/dialog_circular_damper.ui \
    inputDialog/dialog_disp_vent_terminal.ui \
    inputDialog/dialog_duct_with_multi_ranc.ui \
    inputDialog/dialog_elbow.ui \
    inputDialog/dialog_fan.ui \
    inputDialog/dialog_fancoil.ui \
    inputDialog/dialog_other_send_terminal.ui \
    inputDialog/dialog_pipe.ui \
    inputDialog/dialog_prj_manager.ui \
    inputDialog/dialog_pump_send.ui \
    inputDialog/dialog_rect_damper.ui \
    inputDialog/dialog_reducer.ui \
    inputDialog/dialog_silencer.ui \
    inputDialog/dialog_staticBox_grille.ui \
    inputDialog/dialog_static_box.ui \
    inputDialog/dialog_tee.ui \
    inputDialog/dialog_vav_terminal.ui \
    roomCal/outer_after_widget.ui \
    roomCal/outer_before_widget.ui \
    roomCal/room_cal_basewidget.ui \
    roomCal/room_cal_total_widget.ui \
    roomCal/roomcaltable.ui \
    roomDefineForm/dialog_add_room.ui \
    roomDefineForm/dialog_add_system.ui \
    roomDefineForm/dialog_set_name.ui \
    roomDefineForm/room_define_widget.ui \
    roomDefineForm/systemcomp_list_widget.ui \
    widget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    src.qrc
