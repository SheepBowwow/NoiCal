#ifndef TABLE_AIRFLOW_NOI_H
#define TABLE_AIRFLOW_NOI_H

#include <QWidget>

namespace Ui {
class Table_airFlow_noi;
}

class Table_airFlow_noi : public QWidget
{
    Q_OBJECT

public:
    explicit Table_airFlow_noi(QWidget *parent = nullptr);
    ~Table_airFlow_noi();

private:
    Ui::Table_airFlow_noi *ui;
};

#endif // TABLE_AIRFLOW_NOI_H
