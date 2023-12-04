#ifndef TABLE_ATTEN_H
#define TABLE_ATTEN_H

#include <QWidget>

namespace Ui {
class Table_atten;
}

class Table_atten : public QWidget
{
    Q_OBJECT

public:
    explicit Table_atten(QWidget *parent = nullptr);
    ~Table_atten();

private slots:
    void on_stackedWidget_table_currentChanged(int arg1);

    void on_comboBox_unit_name_currentTextChanged(const QString &arg1);

private:
    Ui::Table_atten *ui;
};

#endif // TABLE_ATTEN_H
