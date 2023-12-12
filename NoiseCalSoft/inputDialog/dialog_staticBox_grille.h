#ifndef DIALOG_STATICBOX_GRILLE_H
#define DIALOG_STATICBOX_GRILLE_H

#include "inputbasedialog.h"
#include <QDialog>
#include <QMap>
#include <QObject>
#include <QMouseEvent>
#include "Component/ComponentStructs.h"

namespace Ui {
class Dialog_staticBox_grille;
}

class Dialog_staticBox_grille : public InputBaseDialog
{
    Q_OBJECT

public:
    explicit Dialog_staticBox_grille(QWidget *parent = nullptr, int editRow = -1, const StaticBox_grille_noise& data = StaticBox_grille_noise());
    Dialog_staticBox_grille(const QString& name, QWidget *parent = nullptr, int editRow = -1, const StaticBox_grille_noise& data = StaticBox_grille_noise()){}
    ~Dialog_staticBox_grille();
    void total_noi();
    void* getNoi() override;

private slots:
    void on_close_clicked();

    void on_comboBox_model_currentTextChanged(const QString &arg1);

    void calTotalNoise();

    void calReflNoi();

    void on_pushButton_confirm_clicked();

    void on_radioButton_circle_clicked();

    void on_radioButton_rect_clicked();

    void on_radioButton_known_clicked();

    void on_radioButton_formula_clicked();

    void on_pushButton_clicked();

private:
    Ui::Dialog_staticBox_grille *ui;
    QVector<QString> staticBoxGrilleModel;      //保存所有型号
    QMap<int,QString> eightNoi;     //八个分频的噪音,用于数据库获取数据
    int editRow;    //当前修改行，如果是新建就为-1
    StaticBox_grille_noise* noi;     //噪音结构体
};

#endif // DIALOG_STATICBOX_GRILLE_H
