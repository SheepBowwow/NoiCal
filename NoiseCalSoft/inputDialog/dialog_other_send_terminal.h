#ifndef DIALOG_OTHER_SEND_TERMINAL_H
#define DIALOG_OTHER_SEND_TERMINAL_H

#include "inputbasedialog.h"
#include <QDialog>
#include <QObject>
#include <QMouseEvent>
#include <QLineEdit>
#include "Component/ComponentStructs.h"

namespace Ui {
    class Dialog_other_send_terminal;
}

class Dialog_other_send_terminal : public InputBaseDialog
{
    Q_OBJECT

public:
    explicit Dialog_other_send_terminal(QWidget *parent = nullptr, int editRow = -1, const Other_send_terminal& data = Other_send_terminal());
    Dialog_other_send_terminal(const QString& name, QWidget *parent = nullptr, int editRow = -1, const Other_send_terminal& data = Other_send_terminal()){}
    ~Dialog_other_send_terminal();
private:
    Ui::Dialog_other_send_terminal *ui;
    Other_send_terminal* component;
    int editRow;    //当前修改行，如果是新建就为-1

    array<QLineEdit*,9> noi_lineEdits;
    array<QLineEdit*,8> atten_lineEdits;
    array<QLineEdit*,8> refl_lineEdits;
    QString size;
    QString shape;
    QString table_id{-1};
    QString UUID{QString()};

private slots:
    void on_pushButton_confirm_clicked();
    bool check_lineedit();

    void calTotalNoise();

    void on_close_clicked();

    void on_radioButton_circle_clicked();

    void on_radioButton_rect_clicked();

    // InputBaseDialog interface
    void on_comboBox_refl_data_source_currentIndexChanged(int index);

public:
    QList<QStringList> getComponentDataAsStringList() const override;
    void* getComponent() override;

    // InputBaseDialog interface
public:
    void switchToCompontDB(bool inDB) override {};
};

#endif // DIALOG_OTHER_SEND_TERMINAL_H
