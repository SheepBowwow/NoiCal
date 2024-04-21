#ifndef OUTER_CAL_H
#define OUTER_CAL_H

#include <QWidget>

namespace Ui {
class outer_cal;
}

class outer_cal : public QWidget
{
    Q_OBJECT

public:
    explicit outer_cal(QWidget *parent = nullptr);
    ~outer_cal();

private:
    Ui::outer_cal *ui;
};

#endif // OUTER_CAL_H
