#ifndef DIALOG_CIRCULAR_DAMPER_H
#define DIALOG_CIRCULAR_DAMPER_H

#include "inputbasedialog.h"
#include <QDialog>
#include <QMap>
#include <QObject>
#include <QMouseEvent>
#include <QLineEdit>>
#include "Component/ComponentStructs.h"

namespace Ui {
class Dialog_circular_damper;
}

class Dialog_circular_damper : public InputBaseDialog
{
    Q_OBJECT

public:
    explicit Dialog_circular_damper(QWidget *parent = nullptr, int editRow = -1, const Circular_damper& data = Circular_damper());
    Dialog_circular_damper(const QString &name, QWidget *parent = nullptr, int editRow = -1, const Circular_damper& data = Circular_damper()){}
    ~Dialog_circular_damper();


private slots:

    void on_close_clicked();

    void on_pushButton_confirm_clicked();

    void calTotalNoise();

private:
    Ui::Dialog_circular_damper *ui;
    int editRow;    //当前修改行，如果是新建就为-1
    Circular_damper* component;
    array<QLineEdit*,9> noi_lineEdits;
    QString table_id{-1};
    QString UUID{QString()};

    // InputBaseDialog interface
public:
    QList<QStringList> getComponentDataAsStringList() const override;
    void* getComponent() override;
};

#endif // DIALOG_CIRCULAR_DAMPER_H
