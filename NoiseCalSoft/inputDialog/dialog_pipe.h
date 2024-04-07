#ifndef DIALOG_PIPE_H
#define DIALOG_PIPE_H

#include "inputbasedialog.h"
#include <QDialog>
#include <QMap>
#include <QObject>
#include <QMouseEvent>
#include <QLineEdit>
#include "Component/ComponentStructs.h"

namespace Ui {
class Dialog_pipe;
}

class Dialog_pipe : public InputBaseDialog
{
    Q_OBJECT

public:
    explicit Dialog_pipe(QWidget *parent = nullptr, int editRow = -1, const Pipe& data = Pipe());
    Dialog_pipe(const QString& name, QWidget *parent = nullptr, int editRow = -1, const Pipe& data = Pipe()){}
    ~Dialog_pipe();

private slots:
    void on_close_clicked();

    void on_radioButton_circle_clicked();

    void on_radioButton_rect_clicked();

    void on_pushButton_confirm_clicked();

private:
    Ui::Dialog_pipe *ui;
    int editRow;    //当前修改行，如果是新建就为-1
    Pipe* component;     //噪音结构体
    array<QLineEdit*,8> atten_lineEdits;
    QString table_id{-1};
    QString UUID{QString()};

    // InputBaseDialog interface
public:
    QList<QStringList> getComponentDataAsStringList() const override;
    void* getComponent() override;
};

#endif // DIALOG_PIPE_H
