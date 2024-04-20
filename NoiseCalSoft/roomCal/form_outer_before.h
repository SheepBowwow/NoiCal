#ifndef FORM_OUTER_BEFORE_H
#define FORM_OUTER_BEFORE_H

#include <QWidget>

namespace Ui {
class Form_outer_before;
}

class Form_outer_before : public QWidget
{
    Q_OBJECT

public:
    explicit Form_outer_before(QWidget *parent = nullptr);
    ~Form_outer_before();

private:
    Ui::Form_outer_before *ui;
};

#endif // FORM_OUTER_BEFORE_H
