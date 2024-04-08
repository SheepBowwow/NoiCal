#ifndef DIALOG_STATICBOX_GRILLE_H
#define DIALOG_STATICBOX_GRILLE_H

#include "inputbasedialog.h"
#include <QDialog>
#include <QMap>
#include <QObject>
#include <QMouseEvent>
#include <QLineEdit>
#include "Component/ComponentStructs.h"

namespace Ui {
class Dialog_staticBox_grille;
}

class Dialog_staticBox_grille : public InputBaseDialog
{
    Q_OBJECT

public:
    explicit Dialog_staticBox_grille(QWidget *parent = nullptr, int editRow = -1, const StaticBox_grille& data = StaticBox_grille());
    Dialog_staticBox_grille(const QString& name, QWidget *parent = nullptr, int editRow = -1, const StaticBox_grille& data = StaticBox_grille()){}
    ~Dialog_staticBox_grille();

private slots:
    void on_close_clicked();

    void calTotalNoise();

    void on_pushButton_confirm_clicked();
    bool check_lineedit();

    void on_radioButton_circle_clicked();

    void on_radioButton_rect_clicked();

private:
    Ui::Dialog_staticBox_grille *ui;
    int editRow;    //当前修改行，如果是新建就为-1
    StaticBox_grille* component;     //噪音结构体

    array<QLineEdit*,9> noi_lineEdits;
    array<QLineEdit*,8> atten_lineEdits;
    array<QLineEdit*,8> refl_lineEdits;
    QString size;
    QString shape;
    QString table_id{-1};
    QString UUID{QString()};

    // InputBaseDialog interface
public:
    QList<QStringList> getComponentDataAsStringList() const override;
    void* getComponent() override;
};

#endif // DIALOG_STATICBOX_GRILLE_H
