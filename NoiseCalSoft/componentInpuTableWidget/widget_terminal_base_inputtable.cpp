#include "widget_terminal_base_inputtable.h"
#include "ui_widget_terminal_base_inputtable.h"
#include <QQueue>

Widget_terminal_base_inputTable::Widget_terminal_base_inputTable(QWidget *parent) :
    Widget_base_inputTable(parent,false),
    ui(new Ui::Widget_terminal_base_inputTable)
{
    ui->setupUi(this);
}

Widget_terminal_base_inputTable::~Widget_terminal_base_inputTable()
{
    delete ui;
}

void Widget_terminal_base_inputTable::setTitle(const QString &title)
{
    ui->label_title->setText(title);
    ui->label_title->adjustSize();
}

void Widget_terminal_base_inputTable::setCurrentTable(const int &index)
{
    ui->stackedWidget->setCurrentIndex(index);
}

void Widget_terminal_base_inputTable::clearTableFuc()
{
    // 定义一个Lambda表达式用于递归清空QTableWidgets
    std::function<void(QWidget*)> clearAllTableWidgets = [&](QWidget* parent) {
        if (!parent) return;

        // 创建一个队列用于广度优先搜索
        QQueue<QWidget*> queue;
        queue.enqueue(parent);

        while (!queue.isEmpty()) {
            QWidget* currentWidget = queue.dequeue();

            // 尝试将当前控件转换为QTableWidget
            QTableWidget *tableWidget = qobject_cast<QTableWidget*>(currentWidget);
            if (tableWidget) {
                // 如果转换成功，清空QTableWidget
                tableWidget->setRowCount(0);
                tableWidget->clearContents(); // 清空内容但保留列头
            } else {
                // 如果当前控件不是QTableWidget，将它的子控件加入队列
                const auto children = currentWidget->children();
                for (QObject *child : children) {
                    QWidget *childWidget = qobject_cast<QWidget*>(child);
                    if (childWidget) {
                        queue.enqueue(childWidget);
                    }
                }
            }
        }
    };

    // 使用定义的Lambda表达式从该窗口控件开始递归清空所有QTableWidget
    clearAllTableWidgets(this);
}

void Widget_terminal_base_inputTable::loadComponentToTable()
{
    
}
