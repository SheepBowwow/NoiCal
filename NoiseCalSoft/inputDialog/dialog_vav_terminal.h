#ifndef DIALOG_VAV_TERMINAL_H
#define DIALOG_VAV_TERMINAL_H

#include "inputbasedialog.h"
#include <QDialog>
#include <QObject>
#include <QMouseEvent>
#include "Component/ComponentStructs.h"

namespace Ui {
class Dialog_VAV_terminal;
}

class Dialog_VAV_terminal : public InputBaseDialog
{
    Q_OBJECT

public:
    explicit Dialog_VAV_terminal(QWidget *parent = nullptr, int editRow = -1, const VAV_terminal_noise& data = VAV_terminal_noise());
    Dialog_VAV_terminal(const QString& name, QWidget *parent = nullptr, int editRow = -1, const VAV_terminal_noise& data = VAV_terminal_noise()){}
    ~Dialog_VAV_terminal();
    void* getNoi() override;
    void total_noi();

private:
    Ui::Dialog_VAV_terminal *ui;
    VAV_terminal_noise* noi;
    int editRow;    //当前修改行，如果是新建就为-1

private slots:
    void on_pushButton_confirm_clicked();
    void calTotalNoise();
    void on_close_clicked();
};

#endif // DIALOG_VAV_TERMINAL_H
