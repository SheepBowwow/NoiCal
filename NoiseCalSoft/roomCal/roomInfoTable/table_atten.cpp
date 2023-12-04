#include "table_atten.h"
#include "ui_table_atten.h"

Table_atten::Table_atten(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Table_atten)
{
    ui->setupUi(this);
    // 清除最小和最大大小限制
    this->setMinimumSize(QSize(0, 0));
    this->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
    // 获取当前显示的页面的部件
    QWidget *currentWidget = ui->stackedWidget_table->currentWidget();

    if (currentWidget) {
        // 获取所有子对象
        QList<QObject *> childObjects = currentWidget->children();

        // 遍历所有子对象
        for (QObject *childObject : childObjects) {
            // 判断是否是 QWidget 类型
            if (QWidget *childWidget = qobject_cast<QWidget *>(childObject)) {
                int widgetHeight = childWidget->height();

                // 设置 QStackedWidget 的高度
                ui->stackedWidget_table->setFixedHeight(widgetHeight);

                // 设置窗口的高度
                this->setMinimumSize(QSize(this->width(), 75 + widgetHeight));
                this->setMaximumSize(QSize(this->width(), 75 + widgetHeight));

                // 假设只需要获取第一个符合条件的子部件，可以注释下面这行来继续查找其他子部件
                break;
            }
        }
    }
}

Table_atten::~Table_atten()
{
    delete ui;
}

void Table_atten::on_stackedWidget_table_currentChanged(int arg1)
{
    Q_UNUSED(arg1);

    // 清除最小和最大大小限制
    this->setMinimumSize(QSize(0, 0));
    this->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
    // 获取当前显示的页面的部件
    QWidget *currentWidget = ui->stackedWidget_table->currentWidget();

    if (currentWidget) {
        // 获取所有子对象
        QList<QObject *> childObjects = currentWidget->children();

        // 遍历所有子对象
        for (QObject *childObject : childObjects) {
            // 判断是否是 QWidget 类型
            if (QWidget *childWidget = qobject_cast<QWidget *>(childObject)) {
                int widgetHeight = childWidget->height();

                // 设置 QStackedWidget 的高度
                ui->stackedWidget_table->setFixedHeight(widgetHeight);

                // 设置窗口的高度
                this->setMinimumSize(QSize(this->width(), 75 + widgetHeight));
                this->setMaximumSize(QSize(this->width(), 75 + widgetHeight));

                // 假设只需要获取第一个符合条件的子部件，可以注释下面这行来继续查找其他子部件
                break;
            }
        }
    }
}

void Table_atten::on_comboBox_unit_name_currentTextChanged(const QString &arg1)
{
    if(arg1 == "分支/静压箱")
    {
        ui->stackedWidget_table->setCurrentWidget(ui->page_branch_static);
    }
    else if(arg1 == "直管")
    {
        ui->stackedWidget_table->setCurrentWidget(ui->page_pipe);
    }
}

