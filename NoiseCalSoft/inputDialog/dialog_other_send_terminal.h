#ifndef DIALOG_OTHER_SEND_TERMINAL_H
#define DIALOG_OTHER_SEND_TERMINAL_H

#include "inputbasedialog.h"
#include <QDialog>
#include <QObject>
#include <QMouseEvent>
#include "Component/ComponentStructs.h"

namespace Ui {
    class Dialog_other_send_terminal;
}

class Dialog_other_send_terminal : public InputBaseDialog
{
    Q_OBJECT

public:
    explicit Dialog_other_send_terminal(QWidget *parent = nullptr, int editRow = -1, const Other_send_terminal_noise& data = Other_send_terminal_noise());
    Dialog_other_send_terminal(const QString& name, QWidget *parent = nullptr, int editRow = -1, const Other_send_terminal_noise& data = Other_send_terminal_noise()){}
    ~Dialog_other_send_terminal();
    void* getNoi() override;
    void total_noi();

private:
    Ui::Dialog_other_send_terminal *ui;
    Other_send_terminal_noise* noi;
    int editRow;    //当前修改行，如果是新建就为-1

private slots:
    void on_pushButton_confirm_clicked();

    void calTotalNoise();

    void on_close_clicked();

    void calReflNoi();

    void on_radioButton_circle_clicked();

    void on_radioButton_rect_clicked();

    void on_radioButton_known_clicked();

    void on_radioButton_formula_clicked();
    void on_pushButton_clicked();
};

#endif // DIALOG_OTHER_SEND_TERMINAL_H
