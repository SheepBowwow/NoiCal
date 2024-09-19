#ifndef OUTER_BEFORE_WIDGET_H
#define OUTER_BEFORE_WIDGET_H

#include <QWidget>

namespace Ui {
class Outer_before_widget;
}

class Outer_before_widget : public QWidget
{
    Q_OBJECT

public:
    explicit Outer_before_widget(QWidget *parent = nullptr);
    ~Outer_before_widget();

    void setInfo(QString MVZName, QString deck,
                 QString outerNumer, QString noiseLimit, QString ductNum);    //设置基本信息，主竖区、甲板、房间编号、噪声限值、主风管数量

    void setDuctNum(QString ductNum);
private:
    Ui::Outer_before_widget *ui;
};

#endif // OUTER_BEFORE_WIDGET_H
