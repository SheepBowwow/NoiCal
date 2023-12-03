#ifndef CREATE_PRJ_H
#define CREATE_PRJ_H

#include <QWidget>
#include <QMouseEvent>

namespace Ui {
class create_prj;
}

class create_prj : public QWidget
{
    Q_OBJECT

public:
    explicit create_prj(QWidget *parent = nullptr);
    ~create_prj();
protected:
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
private slots:
    void on_close_clicked();

    void on_pushButton_create_prj_clicked();

private:
    QPoint last;
    int canmove=0;
private:
    Ui::create_prj *ui;

signals:
    void projectCreated(QString projectName);
};

#endif // CREATE_PRJ_H
