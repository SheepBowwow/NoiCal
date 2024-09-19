#ifndef SYSTEMCOMP_LIST_WIDGET_H
#define SYSTEMCOMP_LIST_WIDGET_H

#include <QWidget>

namespace Ui {
class SystemComp_list_widget;
}

class SystemComp_list_widget : public QWidget
{
    Q_OBJECT

public:
    explicit SystemComp_list_widget(QString system_name, QWidget *parent = nullptr);
    ~SystemComp_list_widget();

private slots:
    void on_pushButton_add_clicked();

    void on_pushButton_del_clicked();

    void on_pushButton_revise_clicked();

public slots:
    void addCompToTable(const QString &systemName, const QString &type, const QString &number, const QString &model, const QString &componentUUID, int& table_index);

private:
    Ui::SystemComp_list_widget *ui;
    QString system_name;
};

#endif // SYSTEMCOMP_LIST_WIDGET_H
