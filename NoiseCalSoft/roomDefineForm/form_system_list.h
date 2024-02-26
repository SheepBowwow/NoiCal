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
    explicit Form_system_list(QString system_name, QWidget *parent = nullptr);
    ~Form_system_list();

private slots:
    void on_buttonadd_clicked();

    void on_buttondel_clicked();

    void on_buttonchange_clicked();

private:
    Ui::Form_system_list *ui;
    QString system_name;
};

#endif // FORM_SYSTEM_LIST_H
