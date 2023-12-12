#ifndef DIALOG_SILENCER_H
#define DIALOG_SILENCER_H

#include "inputbasedialog.h"
#include <QDialog>
#include <QMap>
#include <QObject>
#include <QMouseEvent>
#include "Component/ComponentStructs.h"
namespace Ui {
class Dialog_silencer;
}

class Dialog_silencer : public InputBaseDialog
{
    Q_OBJECT

public:
    explicit Dialog_silencer(QString type, QWidget *parent = nullptr, int editRow = -1, const Silencer_atten& data = Silencer_atten());
    Dialog_silencer(QWidget *parent = nullptr, int editRow = -1, const Silencer_atten& data = Silencer_atten()){}
    ~Dialog_silencer();
    void setPrefix();
    void* getNoi() override;

private slots:
    void on_close_clicked();

    void on_pushButton_confirm_clicked();

    void on_comboBox_prefix_currentTextChanged(const QString &arg1);

    void on_comboBox_suffix_currentTextChanged(const QString &arg1);

private:
    Ui::Dialog_silencer *ui;
    QVector<QString> roundSilencerModel;      //圆形消音器
    QVector<QString> rectSilencerModel;      //方形消音器
    QVector<QString> pipeSilencerModel;      //消音管
    QMap<int,QString> eightNoi;     //八个分频的噪音,用于数据库获取数据
    int editRow;    //当前修改行，如果是新建就为-1
    Silencer_atten* noi;     //噪音结构体
};

#endif // DIALOG_SILENCER_H
