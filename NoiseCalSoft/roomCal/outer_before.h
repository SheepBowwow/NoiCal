#ifndef OUTER_BEFORE_H
#define OUTER_BEFORE_H

#include <QWidget>

namespace Ui {
class outer_before;
}

class outer_before : public QWidget
{
    Q_OBJECT

public:
    explicit outer_before(QWidget *parent = nullptr);
    ~outer_before();

    void setInfo(QString zhushuqu, QString deck,
                 QString room_number, QString noise_limit, QString duct_num);    //设置基本信息，主竖区、甲板、房间编号、噪声限值、主风管数量

private:
    Ui::outer_before *ui;
};

#endif // OUTER_BEFORE_H
