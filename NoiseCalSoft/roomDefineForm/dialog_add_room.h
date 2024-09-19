#ifndef DIALOG_ADD_ROOM_H
#define DIALOG_ADD_ROOM_H

#include <QDialog>
#include "inputbasedialog.h"

namespace Ui {
class Dialog_add_room;
}

class Dialog_add_room : public InputBaseDialog
{
    Q_OBJECT

public:
    explicit Dialog_add_room(QString systemOrMVZName, QWidget *parent = nullptr);
    ~Dialog_add_room();

    void setValues(QString roomid, QString roomName, QString deck, QString ductNum, QString placeType, QString roomType, QString limit, QString isCal, QString referenceNumber);

    QString getRoomNumber();
    QString getRoomName();
    QString getDeck();
    QString getDuctNum();
    QString getRoomType();
    QString getLimit();
    QString getPlaceType();
    QString getIsCal();
    QString getReferenceNumber();

    void switchOuterMode();

private slots:
    void on_pushButton_confirm_clicked();

    void on_close_clicked();

    void on_comboBox_room_type_currentTextChanged(const QString &arg1);

    void on_comboBox_place_type_currentTextChanged(const QString &arg1);

    void on_checkBox_is_cal_stateChanged(int arg1);

    void on_comboBox_reference_currentTextChanged(const QString &arg1);

private:
    Ui::Dialog_add_room *ui;
    virtual void * getComponent() override {};
    QString _systemOrMVZName;
    bool isOuter{false};


    // InputBaseDialog interface
public:
    QList<QStringList> getComponentDataAsStringList() const override {};

    // InputBaseDialog interface
public:
    void switchToCompontDB(bool inDB) override {};
};

#endif // DIALOG_ADD_ROOM_H
