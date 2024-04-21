#ifndef OUTER_AFTER_H
#define OUTER_AFTER_H

#include <QWidget>

namespace Ui {
class outer_after;
}

class outer_after : public QWidget
{
    Q_OBJECT

public:
    explicit outer_after(QWidget *parent = nullptr);
    ~outer_after();

    void setInfo(QString zhushuqu, QString deck,
                 QString room_number, QString noise_limit, QString duct_num);    //设置基本信息，主竖区、甲板、房间编号、噪声限值、主风管数量

private:
    Ui::outer_after *ui;
};

#endif // OUTER_AFTER_H
