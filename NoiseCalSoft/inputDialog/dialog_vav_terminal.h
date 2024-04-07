#ifndef DIALOG_VAV_TERMINAL_H
#define DIALOG_VAV_TERMINAL_H

#include "inputbasedialog.h"
#include <QDialog>
#include <QObject>
#include <QMouseEvent>
#include <QLineEdit>
#include "Component/ComponentStructs.h"

namespace Ui {
class Dialog_VAV_terminal;
}

class Dialog_VAV_terminal : public InputBaseDialog
{
    Q_OBJECT

public:
    explicit Dialog_VAV_terminal(QWidget *parent = nullptr, int editRow = -1, const VAV_terminal& data = VAV_terminal());
    Dialog_VAV_terminal(const QString& name, QWidget *parent = nullptr, int editRow = -1, const VAV_terminal& data = VAV_terminal()){}
    ~Dialog_VAV_terminal();

private:
    Ui::Dialog_VAV_terminal *ui;
    VAV_terminal* component;
    int editRow;    //当前修改行，如果是新建就为-1
    array<QLineEdit*,9> noi_lineEdits;
    QString table_id{-1};
    QString UUID{QString()};

private slots:
    void on_pushButton_confirm_clicked();
    void calTotalNoise();
    void on_close_clicked();

    // InputBaseDialog interface
public:
    QList<QStringList> getComponentDataAsStringList() const override;
    void* getComponent() override;
};

#endif // DIALOG_VAV_TERMINAL_H
