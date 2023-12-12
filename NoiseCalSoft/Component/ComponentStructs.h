#ifndef COMPONENTSTRUCTS_H
#define COMPONENTSTRUCTS_H

#include <QString>

class ComponentBase {
public:
    QString table_id;
};

typedef struct AirDiff_noise : ComponentBase
{
    QString number;  //编号
    QString brand;  //品牌
    QString model;   //型号
    QString type;  //类型 圆还是方
    QString diameter;  //直径
    QString length;  //长
    QString width;  //宽
    QString size;  //尺寸
    QString noi_63;     //63hz
    QString noi_125;     //125hz
    QString noi_250;     //250hz
    QString noi_500;     //500hz
    QString noi_1k;     //1khz
    QString noi_2k;     //2khz
    QString noi_4k;     //4khz
    QString noi_8k;     //8khz
    QString noi_total;  //总量
    QString atten_63;     //63hz
    QString atten_125;     //125hz
    QString atten_250;     //250hz
    QString atten_500;     //500hz
    QString atten_1k;     //1khz
    QString atten_2k;     //2khz
    QString atten_4k;     //4khz
    QString atten_8k;     //8khz
    QString refl_63;     //63hz
    QString refl_125;     //125hz
    QString refl_250;     //250hz
    QString refl_500;     //500hz
    QString refl_1k;     //1khz
    QString refl_2k;     //2khz
    QString refl_4k;     //4khz
    QString refl_8k;     //8khz
    QString getMode;    //
}AirDiff_noise;

typedef struct Aircondition_noise : ComponentBase
{
    QString number;  //空调器编号
    QString brand;      //品牌
    QString model;      //型号
    QString air_volume; //风量
    QString static_pressure; //静压
    /*进口*/
    QString noi_in_63;     //63hz
    QString noi_in_125;     //125hz
    QString noi_in_250;     //250hz
    QString noi_in_500;     //500hz
    QString noi_in_1k;     //1khz
    QString noi_in_2k;     //2khz
    QString noi_in_4k;     //4khz
    QString noi_in_8k;     //8khz
    QString noi_in_total;  //总量
    /*出口*/
    QString noi_out_63;     //63hz
    QString noi_out_125;     //125hz
    QString noi_out_250;     //250hz
    QString noi_out_500;     //500hz
    QString noi_out_1k;     //1khz
    QString noi_out_2k;     //2khz
    QString noi_out_4k;     //4khz
    QString noi_out_8k;     //8khz
    QString noi_out_total;  //总量
}Aircondition_noise;

typedef struct Circular_damper_noi : ComponentBase
{
    QString number;  //编号
    QString brand;  //编号
    QString model;  //型号
    QString diameter;   //直径
    QString angle;   //阀门开度
    QString air_volume;   //风量
    QString getMode;   //得到噪声的方式
    QString noi_63;     //63hz
    QString noi_125;     //125hz
    QString noi_250;     //250hz
    QString noi_500;     //500hz
    QString noi_1k;     //1khz
    QString noi_2k;     //2khz
    QString noi_4k;     //4khz
    QString noi_8k;     //8khz
    QString noi_total;     //总值
}Circular_damper_noi;

typedef struct Disp_vent_terminal_noise : ComponentBase
{
    QString number;  //型号
    QString brand;  //型号
    QString model;  //型号
    QString type;  //类型 圆还是方
    QString diameter;  //直径
    QString length;  //长
    QString width;  //宽
    QString size;  //尺寸
    QString noi_63;     //63hz
    QString noi_125;     //125hz
    QString noi_250;     //250hz
    QString noi_500;     //500hz
    QString noi_1k;     //1khz
    QString noi_2k;     //2khz
    QString noi_4k;     //4khz
    QString noi_8k;     //8khz
    QString noi_total;  //总量
    QString atten_63;     //63hz
    QString atten_125;     //125hz
    QString atten_250;     //250hz
    QString atten_500;     //500hz
    QString atten_1k;     //1khz
    QString atten_2k;     //2khz
    QString atten_4k;     //4khz
    QString atten_8k;     //8khz
    QString refl_63;     //63hz
    QString refl_125;     //125hz
    QString refl_250;     //250hz
    QString refl_500;     //500hz
    QString refl_1k;     //1khz
    QString refl_2k;     //2khz
    QString refl_4k;     //4khz
    QString refl_8k;     //8khz
    QString getMode;    //
}Disp_vent_terminal_noise;

typedef struct Multi_ranc_atten : ComponentBase
{
    QString number;  //编号
    QString brand;  //品牌
    QString model;  //型号
    QString getMode;  //得到噪声的方式
    QString q1;       //总风量
    QString q;   //支管风量
    QString noi_63;     //63hz
    QString noi_125;     //125hz
    QString noi_250;     //250hz
    QString noi_500;     //500hz
    QString noi_1k;     //1khz
    QString noi_2k;     //2khz
    QString noi_4k;     //4khz
    QString noi_8k;     //8khz
}Multi_ranc_atten;

typedef struct Elbow_atten : ComponentBase
{
    QString number;  //编号
    QString model;  //型号
    QString type;  //类型 圆还是方
    QString getMode;  //得到噪声的方式
    QString diameter;  //直径
    QString width;  //宽
    QString isLined;  //是否有内衬
    QString noi_63;     //63hz
    QString noi_125;     //125hz
    QString noi_250;     //250hz
    QString noi_500;     //500hz
    QString noi_1k;     //1khz
    QString noi_2k;     //2khz
    QString noi_4k;     //4khz
    QString noi_8k;     //8khz
}Elbow_atten;

typedef struct Fan_noise : ComponentBase
{
    QString number;  //空调器编号
    QString brand;      //品牌
    QString model;      //型号
    QString air_volume; //风量
    QString static_pressure; //静压
    /*进口*/
    QString noi_in_63;     //63hz
    QString noi_in_125;     //125hz
    QString noi_in_250;     //250hz
    QString noi_in_500;     //500hz
    QString noi_in_1k;     //1khz
    QString noi_in_2k;     //2khz
    QString noi_in_4k;     //4khz
    QString noi_in_8k;     //8khz
    QString noi_in_total;  //总量
    /*出口*/
    QString noi_out_63;     //63hz
    QString noi_out_125;     //125hz
    QString noi_out_250;     //250hz
    QString noi_out_500;     //500hz
    QString noi_out_1k;     //1khz
    QString noi_out_2k;     //2khz
    QString noi_out_4k;     //4khz
    QString noi_out_8k;     //8khz
    QString noi_out_total;  //总量
}Fan_noise;

typedef struct FanCoil_noise : ComponentBase
{
    QString number;  //编号
    QString brand;      //品牌
    QString model;      //型号
    QString air_volume; //风量
    QString static_pressure; //静压
    QString type;
    /*进口*/
    QString noi_in_63;     //63hz
    QString noi_in_125;     //125hz
    QString noi_in_250;     //250hz
    QString noi_in_500;     //500hz
    QString noi_in_1k;     //1khz
    QString noi_in_2k;     //2khz
    QString noi_in_4k;     //4khz
    QString noi_in_8k;     //8khz
    QString noi_in_total;  //总量
    /*出口*/
    QString noi_out_63;     //63hz
    QString noi_out_125;     //125hz
    QString noi_out_250;     //250hz
    QString noi_out_500;     //500hz
    QString noi_out_1k;     //1khz
    QString noi_out_2k;     //2khz
    QString noi_out_4k;     //4khz
    QString noi_out_8k;     //8khz
    QString noi_out_total;  //总量
}FanCoil_noise;

typedef struct Other_send_terminal_noise : ComponentBase
{
    QString number;  //编号
    QString brand;  //品牌
    QString model;  //型号
    QString remark; //备注
    QString type;  //类型 圆还是方
    QString diameter;  //直径
    QString length;  //长
    QString width;  //宽
    QString size;  //尺寸
    QString noi_63;     //63hz
    QString noi_125;     //125hz
    QString noi_250;     //250hz
    QString noi_500;     //500hz
    QString noi_1k;     //1khz
    QString noi_2k;     //2khz
    QString noi_4k;     //4khz
    QString noi_8k;     //8khz
    QString noi_total;  //总量
    QString atten_63;     //63hz
    QString atten_125;     //125hz
    QString atten_250;     //250hz
    QString atten_500;     //500hz
    QString atten_1k;     //1khz
    QString atten_2k;     //2khz
    QString atten_4k;     //4khz
    QString atten_8k;     //8khz
    QString refl_63;     //63hz
    QString refl_125;     //125hz
    QString refl_250;     //250hz
    QString refl_500;     //500hz
    QString refl_1k;     //1khz
    QString refl_2k;     //2khz
    QString refl_4k;     //4khz
    QString refl_8k;     //8khz
    QString getMode;    //
}Other_send_terminal_noise;

typedef struct Pipe_atten : ComponentBase
{
    QString number;  //编号
    QString model;  //型号
    QString diameter;   //直径
    QString length;   //长
    QString width;   //宽
    QString type;   //类型
    QString getMode;  //得到噪声的方式
    QString noi_63;     //63hz
    QString noi_125;     //125hz
    QString noi_250;     //250hz
    QString noi_500;     //500hz
    QString noi_1k;     //1khz
    QString noi_2k;     //2khz
    QString noi_4k;     //4khz
    QString noi_8k;     //8khz
}Pipe_atten;

typedef struct PumpSend_noise : ComponentBase
{
    QString number;  //编号
    QString brand;  //品牌
    QString model;   //型号
    QString type;  //类型 圆还是方
    QString diameter;  //直径
    QString length;  //长
    QString width;  //宽
    QString size;  //尺寸
    QString noi_63;     //63hz
    QString noi_125;     //125hz
    QString noi_250;     //250hz
    QString noi_500;     //500hz
    QString noi_1k;     //1khz
    QString noi_2k;     //2khz
    QString noi_4k;     //4khz
    QString noi_8k;     //8khz
    QString noi_total;  //总量
    QString atten_63;     //63hz
    QString atten_125;     //125hz
    QString atten_250;     //250hz
    QString atten_500;     //500hz
    QString atten_1k;     //1khz
    QString atten_2k;     //2khz
    QString atten_4k;     //4khz
    QString atten_8k;     //8khz
    QString refl_63;     //63hz
    QString refl_125;     //125hz
    QString refl_250;     //250hz
    QString refl_500;     //500hz
    QString refl_1k;     //1khz
    QString refl_2k;     //2khz
    QString refl_4k;     //4khz
    QString refl_8k;     //8khz
    QString getMode;    //

}PumpSend_noise;

typedef struct Rect_damper_noi : ComponentBase
{
    QString number;  //编号
    QString brand;  //编号
    QString model;  //型号
    QString length;  //长
    QString width;   //宽
    QString angle;   //叶片角度
    QString air_volume;   //风量
    QString getMode;   //得到噪声的方式
    QString noi_63;     //63hz
    QString noi_125;     //125hz
    QString noi_250;     //250hz
    QString noi_500;     //500hz
    QString noi_1k;     //1khz
    QString noi_2k;     //2khz
    QString noi_4k;     //4khz
    QString noi_8k;     //8khz
    QString noi_total;     //总值
}Rect_damper_noi;

typedef struct Reducer_atten : ComponentBase
{
    QString number;  //编号
    QString model;  //型号
    QString type;  //类型 圆还是方
    QString getMode;  //得到噪声的方式
    QString diameter_before;  //变径前直径
    QString diameter_after;  //变径后直径
    QString length_before;  //变径前长
    QString length_after;  //变径后长
    QString width_before;  //变径前宽
    QString width_after;  //变径后宽
    QString noi_63;     //63hz
    QString noi_125;     //125hz
    QString noi_250;     //250hz
    QString noi_500;     //500hz
    QString noi_1k;     //1khz
    QString noi_2k;     //2khz
    QString noi_4k;     //4khz
    QString noi_8k;     //8khz
}Reducer_atten;

typedef struct StaticBox_grille_noise : ComponentBase
{
    QString number;  //编号
    QString brand;  //品牌
    QString model;   //型号
    QString type;  //类型 圆还是方
    QString diameter;  //直径
    QString length;  //长
    QString width;  //宽
    QString size;  //尺寸
    QString noi_63;     //63hz
    QString noi_125;     //125hz
    QString noi_250;     //250hz
    QString noi_500;     //500hz
    QString noi_1k;     //1khz
    QString noi_2k;     //2khz
    QString noi_4k;     //4khz
    QString noi_8k;     //8khz
    QString noi_total;  //总量
    QString atten_63;     //63hz
    QString atten_125;     //125hz
    QString atten_250;     //250hz
    QString atten_500;     //500hz
    QString atten_1k;     //1khz
    QString atten_2k;     //2khz
    QString atten_4k;     //4khz
    QString atten_8k;     //8khz
    QString refl_63;     //63hz
    QString refl_125;     //125hz
    QString refl_250;     //250hz
    QString refl_500;     //500hz
    QString refl_1k;     //1khz
    QString refl_2k;     //2khz
    QString refl_4k;     //4khz
    QString refl_8k;     //8khz
    QString getMode;    //
}StaticBox_grille_noise;

typedef struct Silencer_atten : ComponentBase
{
    QString number;  //编号
    QString brand;  //品牌
    QString type;       //类型
    QString model_prefix;   //前缀
    QString model_suffix;   //前缀
    QString model;   //型号
    QString noi_63;     //63hz
    QString noi_125;     //125hz
    QString noi_250;     //250hz
    QString noi_500;     //500hz
    QString noi_1k;     //1khz
    QString noi_2k;     //2khz
    QString noi_4k;     //4khz
    QString noi_8k;     //8khz
}Silencer_atten;

typedef struct Static_box : ComponentBase
{
    QString number;  //编号
    QString brand;  //品牌
    QString model;  //型号
    QString getMode;  //得到噪声的方式
    QString q1;       //总风量
    QString q;   //支管风量
    QString noi_63;     //63hz
    QString noi_125;     //125hz
    QString noi_250;     //250hz
    QString noi_500;     //500hz
    QString noi_1k;     //1khz
    QString noi_2k;     //2khz
    QString noi_4k;     //4khz
    QString noi_8k;     //8khz
}Static_box;

typedef struct Tee_atten : ComponentBase
{
    QString number;  //编号
    QString brand;  //品牌
    QString model;  //型号
    QString getMode;  //得到噪声的方式
    QString q1;       //总风量
    QString q;   //支管风量
    QString noi_63;     //63hz
    QString noi_125;     //125hz
    QString noi_250;     //250hz
    QString noi_500;     //500hz
    QString noi_1k;     //1khz
    QString noi_2k;     //2khz
    QString noi_4k;     //4khz
    QString noi_8k;     //8khz
}Tee_atten;

typedef struct VAV_terminal_noise : ComponentBase
{
    QString number;  //编号
    QString brand;  //品牌
    QString model;  //型号
    QString valve_open_degree;  //阀门开度
    QString air_volume;  //风量
    QString noi_63;     //63hz
    QString noi_125;     //125hz
    QString noi_250;     //250hz
    QString noi_500;     //500hz
    QString noi_1k;     //1khz
    QString noi_2k;     //2khz
    QString noi_4k;     //4khz
    QString noi_8k;     //8khz
    QString noi_total;  //总量
}VAV_terminal_noise;


#endif // COMPONENTSTRUCTS_H
