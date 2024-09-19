#ifndef OUTER_AFTER_WIDGET_H
#define OUTER_AFTER_WIDGET_H

#include <QWidget>

namespace Ui {
class Outer_after_widget;
}

class Outer_after_widget : public QWidget
{
    Q_OBJECT

public:
    explicit Outer_after_widget(QWidget *parent = nullptr);
    ~Outer_after_widget();

    void setInfo(QString MVZName, QString deck,
                 QString outerNumer, QString noiseLimit, QString ductNum);    //设置基本信息，主竖区、甲板、房间编号、噪声限值、主风管数量

    void setDuctNum(QString ductNum);

private:
    Ui::Outer_after_widget *ui;
};

#endif // OUTER_AFTER_WIDGET_H
