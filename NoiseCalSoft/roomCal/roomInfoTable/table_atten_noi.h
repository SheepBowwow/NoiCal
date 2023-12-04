#ifndef TABLE_ATTEN_NOI_H
#define TABLE_ATTEN_NOI_H

#include <QWidget>

namespace Ui {
class Table_atten_noi;
}

class Table_atten_noi : public QWidget
{
    Q_OBJECT

public:
    explicit Table_atten_noi(QWidget *parent = nullptr);
    ~Table_atten_noi();

private:
    Ui::Table_atten_noi *ui;
};

#endif // TABLE_ATTEN_NOI_H
