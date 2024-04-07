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

//计算噪音总值dB(A)
double calNoiseTotalValue(const std::array<QLineEdit*, 9>& lineEdits);

//调风门噪音计算
QList<double> calCircularDamperNoise(ShapType shape, int angle, double air_volume, double dimension1, double dimension2);

//末端反射衰减计算
QList<double> calTerminalReflNoise(QString type, QString size);

//分支衰减计算
QList<double> calBranchNoise(double q1, double q);


//弯头衰减计算
QList<double> calElbowNoise(ShapType shape, double dimension);
