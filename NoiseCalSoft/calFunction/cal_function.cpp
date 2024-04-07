#include "calFunction/cal_function.h"
#define Pi 3.14159265358979323846

// 注意：此函数的声明需要修改为返回double类型，以便返回计算结果
double calNoiseTotalValue(const std::array<QLineEdit*, 9>& lineEdits) {
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

QList<double> calCircularDamperNoise(ShapType shape, int angle, double air_volume, double dimension1, double dimension2)
{
    QList<double> result;

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
        area = dimension1 * dimension2;

    for(int i = 0; i < 8; i++)
    {
        double res = L + 10 * log10(area)
                + 55 * log10(air_volume / area / 3600) + fixNumber[angle][i];
        result.push_back(std::round(res * 10.0) / 10.0);
    }

    return result;
}

QList<double> calTerminalReflNoise(QString type, QString size)
{
    QList<double> res;

    double D = 0;
    if(type == "圆形")
    {
        D = size.toDouble() / 1000.0;
    }
    else if(type == "方形")
    {
        QStringList parts = size.split('X'); // 使用split函数按照斜杠分割字符串
        // parts中的第一个元素是前缀，第二个元素是后缀（如果有的话）
        double length = parts.value(0).trimmed().toDouble(); // 去除前缀两边的空格
        double width = parts.value(1).trimmed().toDouble(); // 去除后缀两边的空格
        D = sqrt((4 * length / 1000.0) * (width / 1000.0) / Pi);
    }

    double f[] = {63,125,250,500,1000,2000,4000,8000};

    for(int i = 0; i < 8; i++)
    {
        double result = 0;
        result = 10 * log10(1 + pow(((0.7 * 340) / (Pi * f[i] * D)),2));
        res.push_back(result);
    }

    return res;
}

QList<double> calBranchNoise(double q1, double q)
{
    QList<double> res;

    double noise = 10 * log10(q1 / q);


    for(int i = 0; i < 8; i++)
    {
        res.push_back(-noise);
    }

    return res;
}

QList<double> calculateNoiseLevels(ShapType shape, double dimension)
{
    QList<double> results;
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

        results.push_back(result);
    }

    return results;
}

/*
//计算总值 槽函数实现
void Dialog_pipe::calNoise()
{
    if(ui->radioButton_circle->isChecked() && ui->lineEdit_diameter->text().isEmpty())
    {
        return;
    }
    else if(ui->radioButton_rect->isChecked() && (ui->lineEdit_length->text().isEmpty() || ui->lineEdit_width->text().isEmpty()))
    {
        return;
    }
    double pa = 0, c = 0, s = 0;
    double noi[8];
    QVector<QLineEdit*> v_lineEdit = {ui->lineEdit_63, ui->lineEdit_125, ui->lineEdit_250, ui->lineEdit_500, ui->lineEdit_1k, ui->lineEdit_2k, ui->lineEdit_4k, ui->lineEdit_8k};
    if(ui->radioButton_circle->isChecked())
    {
        if(0 < ui->lineEdit_diameter->text().toDouble() && ui->lineEdit_diameter->text().toDouble() <= 180)
        {
            noi[0] = 0.03;
            noi[1] = 0.03;
            noi[2] = 0.05;
            noi[3] = 0.1;
            noi[4] = 0.1;
            noi[5] = 0.;
            noi[6] = 0.33;
            noi[7] = 0.33;
        }
        else if(180 < ui->lineEdit_diameter->text().toDouble() && ui->lineEdit_diameter->text().toDouble() <= 380)
        {
            noi[0] = 0.1;
            noi[1] = 0.1;
            noi[2] = 0.1;
            noi[3] = 0.16;
            noi[4] = 0.23;
            noi[5] = 0.23;
            noi[6] = 0.23;
            noi[7] = 0.23;
        }
        else if(180 < ui->lineEdit_diameter->text().toDouble() && ui->lineEdit_diameter->text().toDouble() <= 380)
        {
            noi[0] = 0.07;
            noi[1] = 0.07;
            noi[2] = 0.07;
            noi[3] = 0.1;
            noi[4] = 0.16;
            noi[5] = 0.16;
            noi[6] = 0.16;
            noi[7] = 0.16;
        }
        else if(180 < ui->lineEdit_diameter->text().toDouble() && ui->lineEdit_diameter->text().toDouble() <= 380)
        {
            noi[0] = 0.03;
            noi[1] = 0.03;
            noi[2] = 0.03;
            noi[3] = 0.07;
            noi[4] = 0.07;
            noi[5] = 0.07;
            noi[6] = 0.07;
            noi[7] = 0.07;
        }
    }
    else if(ui->radioButton_rect->isChecked())
    {
        c = 2 * (ui->lineEdit_length->text().toDouble() + ui->lineEdit_width->text().toDouble());
        s = ui->lineEdit_length->text().toDouble() * ui->lineEdit_width->text().toDouble();

        pa = c / s;

        if(pa <= 0.002)
        {
            noi[0] = 165 * pa;
            noi[1] = 165 * pa;
            noi[2] = 80 * pa;
            noi[3] = 35 * pa;
            noi[4] = 35 * pa;
            noi[5] = 35 * pa;
            noi[6] = 35 * pa;
            noi[7] = 35 * pa;
        }
        else if(pa > 0.002 && pa <= 0.003)
        {
            noi[0] = 160 * pa + 0.01;
            noi[1] = 0.33;
            noi[2] = 70 * pa + 0.02;
            noi[3] = 0.07;
            noi[4] = 0.07;
            noi[5] = 0.07;
            noi[6] = 0.07;
            noi[7] = 0.07;
        }
        else if(pa > 0.003 && pa <= 0.007)
        {
            noi[0] = 82.5 * pa + 0.2425;
            noi[1] = 82.5 * pa;
            noi[2] = 25 * pa + 0.155;
            noi[3] = 7.5 * pa + 0.0475;
            noi[4] = 7.5 * pa + 0.0475;
            noi[5] = 7.5 * pa + 0.0475;
            noi[6] = 7.5 * pa + 0.0475;
            noi[7] = 7.5 * pa + 0.0475;
        }
        else if(pa > 0.007 && pa <= 0.01)
        {
            noi[0] = 170 * pa - 0.39;
            noi[1] = 0.66;
            noi[2] = 0.33;
            noi[3] = 20 * pa + 0.04;
            noi[4] = 20 * pa + 0.04;
            noi[5] = 20 * pa + 0.04;
            noi[6] = 20 * pa + 0.04;
            noi[7] = 20 * pa + 0.04;
        }
        else if(pa > 0.01 && pa <= 0.013)
        {
            noi[0] = -53.3 * pa + 1.84;
            noi[1] = 0.66;
            noi[2] = 0.33;
            noi[3] = 13.3 * pa + 0.027;
            noi[4] = 13.3 * pa + 0.027;
            noi[5] = 13.3 * pa + 0.027;
            noi[6] = 13.3 * pa + 0.027;
            noi[7] = 13.3 * pa + 0.027;
        }
        else if(pa > 0.013 && pa <= 0.026)
        {
            noi[0] = -13.1 * pa + 1.32;
            noi[1] = 0.66;
            noi[2] = 0.33;
            noi[3] = 10 * pa + 0.07;
            noi[4] = 10 * pa + 0.07;
            noi[5] = 10 * pa + 0.07;
            noi[6] = 10 * pa + 0.07;
            noi[7] = 10 * pa + 0.07;
        }
        else if(pa > 0.026)
        {
            noi[0] = 0.98;
            noi[1] = 0.66;
            noi[2] = 0.33;
            noi[3] = 0.33;
            noi[4] = 0.33;
            noi[5] = 0.33;
            noi[6] = 0.33;
            noi[7] = 0.33;
        }
    }



    for(int i = 0; i < 8; i++)
    {
        v_lineEdit[i]->setText(QString::number(noi[i],'f',2));
    }
}

*/

/*
//计算总值 槽函数实现
void Dialog_reducer::calNoise() {
//    double s1 = 0,s2 = 0,noise = 0;;
//    if(ui->radioButton_circle->isChecked())
//    {
//        if(ui->lineEdit_diameter_before->text().isEmpty() || ui->lineEdit_diameter_after->text().isEmpty())
//        {
//            return;
//        }
//        s1 = Pi * (ui->lineEdit_diameter_before->text().toDouble() / 2000.0) * (ui->lineEdit_diameter_before->text().toDouble()/2000.0);
//        s2 = Pi * (ui->lineEdit_diameter_after->text().toDouble()/2000.0) * (ui->lineEdit_diameter_after->text().toDouble()/2000.0);
//    }
//    else if(ui->radioButton_rect->isChecked())
//    {
//        if(ui->lineEdit_length_before->text().isEmpty() || ui->lineEdit_length_after->text().isEmpty()
//                || ui->lineEdit_width_before->text().isEmpty() || ui->lineEdit_width_after->text().isEmpty())
//        {
//            return;
//        }
//        s1 = (ui->lineEdit_length_before->text().toDouble()/1000.0) * (ui->lineEdit_width_before->text().toDouble()/1000.0);
//        s2 = (ui->lineEdit_length_after->text().toDouble()/1000.0) * (ui->lineEdit_width_after->text().toDouble()/1000.0);
//    }


//    noise = 10 * log10((pow((s2/s1 + 1),2)) / (4 * (s2 / s1)));

//    QString str_noise = QString::number(noise, 'f', 1);
//    ui->lineEdit_63->setText(str_noise);
//    ui->lineEdit_125->setText(str_noise);
//    ui->lineEdit_250->setText(str_noise);
//    ui->lineEdit_500->setText(str_noise);
//    ui->lineEdit_1k->setText(str_noise);
//    ui->lineEdit_2k->setText(str_noise);
//    ui->lineEdit_4k->setText(str_noise);
//    ui->lineEdit_8k->setText(str_noise);
}
 */
