#ifndef FORM_SYSTEM_LIST_H
#define FORM_SYSTEM_LIST_H

#include <QWidget>

namespace Ui {
class Form_system_list;
}

class Form_system_list : public QWidget
{
    Q_OBJECT

public:
    explicit Form_system_list(QWidget *parent = nullptr);
    ~Form_system_list();

private:
    Ui::Form_system_list *ui;
};

#endif // FORM_SYSTEM_LIST_H
