#include "calFunction/cal_function.h"
#define Pi 3.14159265358979323846
#include <QDebug>

// 注意：此函数的声明需要修改为返回double类型，以便返回计算结果
double calNoiseTotalValue(const array<QLineEdit*, 9>& lineEdits) {
    QVector<double> noises;
    QVector<double> weightings = {-26.2, -16.1, -8.6, -3.2, 0, 1.2, 1, -1.1};

    // 检查前8个lineEdit是否非空并转换为double值
    for(int i = 0; i < 8; ++i) {
        bool ok;
        double value = lineEdits[i]->text().toDouble(&ok);
        if (!ok) {
            // 如果任何一个lineEdit无法转换为double，返回0或处理错误
            return 0.0;
        }
        noises.push_back(value);
    }

    double tmp = 0.0;
    for(int i = 0; i < noises.size(); ++i) {
        tmp += pow(10.0, (noises[i] + weightings[i]) / 10.0);
    }

    // 计算总值
    return 10.0 * log10(tmp);
}

double calNoiseTotalValue(const array<double, 9>& nois)
{
    QVector<double> weightings = {-26.2, -16.1, -8.6, -3.2, 0, 1.2, 1, -1.1};

    double tmp = 0.0;
    for(int i = 0; i < nois.size() - 1; ++i) {
        tmp += pow(10.0, (nois[i] + weightings[i]) / 10.0);
    }

    // 计算总值
    return 10.0 * log10(tmp);
}

array<double, 9> calDamperNoise(ShapType shape, int angle, double air_volume, double dimension1, double dimension2)
{
    array<double, 9> results;

    QMap<int,QVector<int>> fixNumber;   //修正值
    fixNumber[0] = { -4, -5, -5, -9, -14, -19, -24, -29};
    fixNumber[45] = { -7, -5, -6, -9, -13, -12, -7, -13};
    fixNumber[65] = { -10, -7, -4, -5, -9, 0, -3, -10};

    int L =  -1;
    if(angle == 0)
        L = 30;
    else if(angle = 45)
        L = 42;
    else if(angle = 65)
        L = 51;

    double area = 0;
    if(shape == Circle)
        area = Pi * (dimension1 / 1000.0 / 2) * (dimension1 / 1000.0 / 2);
    else if(shape == Rect)
        area = (dimension1 / 1000.0) * (dimension2 / 1000.0);

    for(int i = 0; i < 8; i++)
    {
        double res = L + 10 * log10(area)
                + 55 * log10(air_volume / area / 3600) + fixNumber[angle][i];
        results[i] = (std::round(res * 10.0) / 10.0);
    }

    results[8] = calNoiseTotalValue(results);

    return results;
}

array<double, 8> calTerminalReflNoise(ShapType shape, const double& dimension1, const double& dimension2)
{
    array<double, 8> results;

    double D = 0;
    if(shape == Circle)
    {
        D = dimension1 / 1000.0;
    }
    else if(shape == Rect)
    {
        D = sqrt(4 * (dimension1 / 1000.0) * (dimension2 / 1000.0) / Pi);
    }

    double f[] = {63,125,250,500,1000,2000,4000,8000};

    for(int i = 0; i < 8; i++)
    {
        double result = 0;
        result = 10 * log10(1 + pow(((0.7 * 340) / (Pi * f[i] * D)),2));
        results[i] = result;
    }

    return results;
}

array<double, 8> calBranchNoise(double q1, double q)
{
    double noise = 10 * log10(q1 / q);

    array<double, 8> res;
    for(int i = 0; i < res.size(); i++) {
        res[i] = noise;
    }

    return res;
}

array<double, 8> calElbowNoise(ShapType shape, double dimension)
{
    array<double, 8> results;
    double f[] = {0.063, 0.125, 0.250, 0.500, 1, 2, 4, 8};
    double fw;

    for(int i = 0; i < 8; ++i) {
        fw = f[i] * dimension;
        double result = 0;

        switch (shape) {
            case Circle:
                if (fw < 48) result = 0;
                else if (fw < 96) result = 1;
                else if (fw < 190) result = 2;
                else result = 3;
                break;

            case RectangleLined:
                if (fw < 48) result = 0;
                else if (fw < 96) result = 1;
                else if (fw < 190) result = 6;
                else if (fw < 380) result = 11;
                else if (fw < 760) result = 10;
                else result = 10;
                break;

            case RectangleUnlined:
                if (fw < 48) result = 0;
                else if (fw < 96) result = 1;
                else if (fw < 190) result = 5;
                else if (fw < 380) result = 8;
                else if (fw < 760) result = 4;
                else result = 3;
                break;
        }

        results[i] = result;
    }

    return results;
}


array<double, 8> caPipeNoise(ShapType shape, const double& dimension1, const double& dimension2)
{
    double pa = 0, c = 0, s = 0;
    array<double, 8> results;

    if(shape == Circle)
    {
        if(dimension1 <= 152)
        {
            results = {0.03, 0.03, 0.05, 0.05, 0.12, 0.1, 0.1, 0.11};
        }
        else if(152 < dimension1 && dimension1 <= 254)
        {
            results = {0.03, 0.03, 0.04, 0.05, 0.11, 0.09, 0.09, 0.1};
        }
        else if(254 < dimension1 && dimension1 <= 381)
        {
            results = {0.03, 0.03, 0.03, 0.05, 0.09, 0.07, 0.07, 0.08};
        }
        else if(381 < dimension1 && dimension1 <= 559)
        {
            results = {0.02, 0.02, 0.02, 0.04, 0.08, 0.06, 0.06, 0.07};
        }
        else if(559 < dimension1 && dimension1 <= 762)
        {
            results = {0.02, 0.02, 0.02, 0.03, 0.07, 0.05, 0.05, 0.06};
        }
        else if(762 < dimension1 && dimension1 <= 1219)
        {
            results = {0.01, 0.01, 0.01, 0.02, 0.05, 0.03, 0.03, 0.04};
        }
        else if(1219 < dimension1)
        {
            results = {0.01, 0.01, 0.01, 0.02, 0.05, 0.03, 0.03, 0.04};
        }
    }
    else if(shape == Rect)
    {
        c = 2 * (dimension1 + dimension2);
        s = dimension1 * dimension2;

        pa = c / s;

        if(pa <= 0.002)
        {
            results = {165 * pa, 165 * pa, 80 * pa, 35 * pa, 35 * pa, 35 * pa, 35 * pa, 35 * pa};
        }
        else if(pa > 0.002 && pa <= 0.003)
        {
            results = {160 * pa + 0.01, 0.33, 70 * pa + 0.02, 0.07, 0.07, 0.07, 0.07, 0.07};
        }
        else if(pa > 0.003 && pa <= 0.007)
        {
            results = {82.5 * pa + 0.2425, 82.5 * pa, 25 * pa + 0.155, 7.5 * pa + 0.0475, 7.5 * pa + 0.0475, 7.5 * pa + 0.0475, 7.5 * pa + 0.0475, 7.5 * pa + 0.0475};
        }
        else if(pa > 0.007 && pa <= 0.01)
        {
            results = {170 * pa - 0.39, 0.66, 0.33, 20 * pa + 0.04, 20 * pa + 0.04, 20 * pa + 0.04, 20 * pa + 0.04, 20 * pa + 0.04};
        }
        else if(pa > 0.01 && pa <= 0.013)
        {
            results = {-53.3 * pa + 1.84, 0.66, 0.33, 13.3 * pa + 0.027, 13.3 * pa + 0.027, 13.3 * pa + 0.027, 13.3 * pa + 0.027, 13.3 * pa + 0.027};
        }
        else if(pa > 0.013 && pa <= 0.026)
        {
            results = {-13.1 * pa + 1.32, 0.66, 0.33, 10 * pa + 0.07, 10 * pa + 0.07, 10 * pa + 0.07, 10 * pa + 0.07, 10 * pa + 0.07};
        }
        else if(pa > 0.026)
        {
            results = {0.98, 0.66, 0.33, 0.33, 0.33, 0.33, 0.33, 0.33};
        }
    }
    return results;
}


//变径
array<double, 8> calReducerNoise(const QString& type, double dimension1, double dimension2, double dimension3, double dimension4) {
    array<double, 8> results;
    double s1 = 0,s2 = 0, noise = 0;
    if(type == "圆-圆")
    {
        s1 = Pi * (dimension1 / 2000.0) * (dimension1 /2000.0);
        s2 = Pi * (dimension3 /2000.0) * (dimension3 /2000.0);
    }
    else if(type == "方-方")
    {
        s1 = Pi * (dimension1 / 1000.0) * (dimension2 /1000.0);
        s2 = Pi * (dimension3 / 1000.0) * (dimension4 /1000.0);
    }
    else if(type == "圆-方")
    {
        s1 = Pi * (dimension1 / 2000.0) * (dimension1 /2000.0);
        s2 = Pi * (dimension3 / 1000.0) * (dimension4 /1000.0);
    }
    else if(type == "方-圆")
    {
        s1 = Pi * (dimension1 / 1000.0) * (dimension2 /1000.0);
        s2 = Pi * (dimension3 / 2000.0) * (dimension3 /2000.0);
    }


    noise = 10 * log10((pow((s2/s1 + 1),2)) / (4 * (s2 / s1)));

    for(int i = 0; i < results.size(); i++) {
        results[i] = noise;
    }
    return results;
}

//计算总值
double noiseSuperposition(double num1, double num2)
{
    double tmp1 = pow(10, num1 / 10);
    double tmp2 = pow(10, num2 / 10);

    // 计算总值
    return 10.0 * log10(tmp1 + tmp2);

}

array<double, 8> calRoomLess425Revise(const QString &test_distance_str, const QString &volume_str)
{
    array<double, 8> res;
    double f[] = {63,125,250,500,1000,2000,4000,8000};
    double test_distance = test_distance_str.toDouble();
    double volume = volume_str.toDouble();
    for(auto i = 0; i < 8; i++) {
        res[i] = - 10 * log10(test_distance) - 5 * log10(volume) - 3 * log10(f[i]) + 12;
    }

    return res;
}

array<double, 8> calRoomMore425Revise(const QString &test_distance_str)
{
    array<double, 8> res;
    double test_distance = test_distance_str.toDouble();
    if(test_distance < 0.9 || test_distance > 1.5) {
        qDebug() << "test_distance 不在 0.9 和 1.5 之间";
        return res;
    }
    array<double, 8> C;
    array<double, 8> C1 = {5, 5, 6, 6, 6, 7, 10, 0};
    array<double, 8> C2 = {6, 7, 7, 7, 8, 9, 12, 0};
    array<double, 8> C3 = {7, 8, 8, 8, 9, 11, 14, 0};
    if(test_distance == 0.9 || test_distance == 1) {
        C = C1;
    }
    else if(test_distance == 1.2) {
        C = C2;
    }
    else if(test_distance == 1.5) {
        C = C3;
    }
    else if(test_distance > 0.9 && test_distance < 1.2) {
        for(auto i = 0; i < 8; i++) {
            C[i] = C1[i] + (test_distance - 0.9) * (C2[i] - C1[i]) / (1.2 - 0.9);
        }
    }
    else if(test_distance > 1.2 && test_distance < 1.5) {
        for(auto i = 0; i < 8; i++) {
            C[i] = C2[i] + (test_distance - 1.2) * (C3[i] - C2[i]) / (1.5 - 1.2);
        }
    }

    for(auto i = 0; i < 8; i++) {
        res[i] = - C[i] - 5;
    }

    return res;
}

array<double, 8> calRoomNoFurnitureRevise(const QString &test_distance_str,
                                          const QString &angle_str, const QString &absorption_rate_str, const QString &area_str)
{
    array<double, 8> res;
    double S = area_str.toDouble();
    double r = test_distance_str.toDouble();
    double Q = angle_str.toDouble();
    double a = absorption_rate_str.toDouble();
    double R = S * a / (1 - a);

    for(auto i = 0; i < 8; i++) {
        res[i] = log10(Q / 4 * Pi * r * r + 4 / R);
    }

    return res;
}

array<double, 8> calRoomOpenRevise(const QString &test_distance_str, const QString &angle_str)
{
    array<double, 8> res;
    double r = test_distance_str.toDouble();
    double Q = angle_str.toDouble();

    for(auto i = 0; i < 8; i++) {
        res[i] = log10(Q / 4 * Pi * r * r);
    }

    return res;
}

array<double, 8> calRoomGapTuyereRevise(const QString &test_distance_str,
                                        const QString &angle_str, const QString &absorption_rate_str, const QString &area_str,
                                        const QString &gap_length_str)
{
    array<double, 8> res;
    double S = area_str.toDouble();
    double r = test_distance_str.toDouble();
    double Q = angle_str.toDouble();
    double a = absorption_rate_str.toDouble();
    double R = S * a / (1 - a);
    double L = gap_length_str.toDouble();

    for(auto i = 0; i < 8; i++) {
        res[i] = log10(Q / Pi * r * L + 4 / R);
    }

    return res;
}

array<double, 8> calRoomRainRevise(const QString &test_distance_str, const QString &rain_area_str)
{
    array<double, 8> res;
    //9.3 < area < 14
    array<double, 8> D1 = {2, 3, 4, 5, 6, 7, 8, 0};
    //18.5 < area < 23
    array<double, 8> D2 = {3, 4, 5, 6, 7, 8, 9, 0};
    array<double, 8> D;
    double area = rain_area_str.toDouble();
    if(area > 9.3 && area < 14) {
        D = D1;
    } else if(area > 18.5 && area < 23) {
        D = D2;
    } else {
        qDebug() << "area is illegal";
    }

    for(auto i = 0; i < 8; i++) {
        res[i] = - D[i];
    }

    return res;
}
