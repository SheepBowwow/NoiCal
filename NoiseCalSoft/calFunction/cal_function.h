#ifndef CAL_FUNCTION_H
#define CAL_FUNCTION_H

#include<cmath>
#include<QObject>
#include<QLineEdit>

enum ShapType
{
    Circle,
    Rect,
    RectangleLined,
    RectangleUnlined
};

using std::array;

//计算噪音总值dB(A)
double calNoiseTotalValue(const array<QLineEdit*, 9>& lineEdits);

double calNoiseTotalValue(const array<double, 9>& lineEdits);

//调风门噪音计算
array<double, 9> calDamperNoise(ShapType shape, int angle, double air_volume, double dimension1, double dimension2 = -1);

//末端反射衰减计算
array<double, 8> calTerminalReflNoise(ShapType shape, const double& dimension1, const double& dimension2 = -1);

//分支衰减计算
array<double, 8> calBranchNoise(double q1, double q);


//弯头衰减计算
array<double, 8> calElbowNoise(ShapType shape, double dimension);

//直管衰减计算
array<double, 8> caPipeNoise(ShapType shape, const double& dimension1, const double& dimension2 = -1);

//变径
array<double, 8> calReducerNoise(const QString& type, double dimension1, double dimension2, double dimension3, double dimension4);

double noiseSuperposition(double num1, double num2);

//计算房间修正值
array<double, 8> calRoomLess425Revise(const QString& test_distance_str, const QString& volume_str);

//计算房间修正值
array<double, 8> calRoomMore425Revise(const QString& test_distance_str);

//计算房间修正值
array<double, 8> calRoomNoFurnitureRevise(const QString& test_distance_str, const QString& angle_str,
                                          const QString& absorption_rate_str, const QString& area_str);

//计算房间修正值
array<double, 8> calRoomOpenRevise(const QString& test_distance_str, const QString& angle_str);

//计算房间修正值
array<double, 8> calRoomGapTuyereRevise(const QString& test_distance_str, const QString& angle_str,
                                        const QString& absorption_rate_str, const QString& area_str, const QString& gap_length_str);

//计算房间修正值
array<double, 8> calRoomRainRevise(const QString& test_distance_str, const QString& rain_area_str);

#endif
