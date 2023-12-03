#ifndef TABLE_NOI_SRC_H
#define TABLE_NOI_SRC_H

#include <QWidget>

namespace Ui {
class Table_noi_src;
}

class Table_noi_src : public QWidget
{
    Q_OBJECT

public:
    explicit Table_noi_src(QWidget *parent = nullptr);
    ~Table_noi_src();

private:
    Ui::Table_noi_src *ui;
};

#endif // TABLE_NOI_SRC_H
