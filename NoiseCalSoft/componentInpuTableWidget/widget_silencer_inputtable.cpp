#include "widget_silencer_inputtable.h"
#include "ui_widget_silencer_inputtable.h"
#include "inputDialog/dialog_silencer.h"
#include <QQueue>

Widget_Silencer_inputTable::Widget_Silencer_inputTable(QWidget *parent) :
    Widget_base_inputTable(parent),
    ui(new Ui::Widget_Silencer_inputTable)
{
    ui->setupUi(this);

    silencer_type = silencer_type_name::CIRCLE;
    on_pushButton_circle_clicked();
    connect(ui->pushButton_add, &QPushButton::clicked, this, &Widget_Silencer_inputTable::onAdd);
    connect(ui->pushButton_del, &QPushButton::clicked, this, &Widget_Silencer_inputTable::onDel);
    connect(ui->pushButton_revise, &QPushButton::clicked, this, &Widget_Silencer_inputTable::onRevise);
    connect(ui->pushButton_input, &QPushButton::clicked, this, &Widget_Silencer_inputTable::onInput);
    connect(ui->pushButton_output, &QPushButton::clicked, this, &Widget_Silencer_inputTable::onOutput);
}

Widget_Silencer_inputTable::~Widget_Silencer_inputTable()
{
}

void Widget_Silencer_inputTable::initTableWidget()
{
    int colCount = 14;
    QStringList headerText;
    headerText<< "" << "序号" << "型号" << "品牌" << "63Hz\n(dB)" << "125Hz\n(dB)" << "250Hz\n(dB)"
                    << "500Hz\n(dB)" << "1kHz\n(dB)" << "2kHz\n(dB)" << "4kHz\n(dB)" << "8kHz\n(dB)"
                    << "来源" << "UUID";  //表头标题用QStringList来表示
    int columnWidths[] = {30, 40, 180, 80, 55, 55, 55, 55, 55, 55, 55, 55, 60, 0};

    setTableWidget(ui->tableWidget_circle, headerText, columnWidths, colCount);
    setTableWidget(ui->tableWidget_rect, headerText, columnWidths, colCount);
    setTableWidget(ui->tableWidget_circle_elbow, headerText, columnWidths, colCount);
    setTableWidget(ui->tableWidget_rect_elbow, headerText, columnWidths, colCount);
    // 隐藏最后一列
    ui->tableWidget_circle->setColumnHidden(colCount - 1, true);
    ui->tableWidget_rect->setColumnHidden(colCount - 1, true);
    ui->tableWidget_circle_elbow->setColumnHidden(colCount - 1, true);
    ui->tableWidget_rect_elbow->setColumnHidden(colCount - 1, true);
}

void Widget_Silencer_inputTable::onAdd()
{
    QTableWidget* tableWidget = ui->stackedWidget->currentWidget()->findChild<QTableWidget*>();
    Dialog_silencer *dialog = new Dialog_silencer(silencer_type, nullptr);
    QSharedPointer<Silencer> component;

    if (dialog->exec() == QDialog::Accepted) {
        if(Silencer* rawPointer = static_cast<Silencer*>(dialog->getComponent()))
            component = QSharedPointer<Silencer>(rawPointer);
        else
            return;
        component->table_id = QString::number(tableWidget->rowCount() + 1);
        if (component != nullptr) {
            auto lists = dialog->getComponentDataAsStringList();

            // 使用通用函数添加行
            addRowToTable(tableWidget, lists[0]);

            componentManager.addComponent(component);
        }
    }
}

void Widget_Silencer_inputTable::onDel()
{
    QTableWidget* tableWidget = ui->stackedWidget->currentWidget()->findChild<QTableWidget*>();
    deleteRowFromTable(tableWidget, 1);
}

void Widget_Silencer_inputTable::onRevise()
{
    QTableWidget* tableWidget = ui->stackedWidget->currentWidget()->findChild<QTableWidget*>();
    for (int row = 0; row < tableWidget->rowCount(); ++row)
    {
        // 假设你的复选框在第一列
        QWidget* widget = tableWidget->cellWidget(row, 0);
        QCheckBox* checkBox = widget ? qobject_cast<QCheckBox*>(widget) : nullptr;
        if(checkBox && checkBox->isChecked())
        {
            // 调用通用的修订函数，传入正确的类型参数
            componentRevision<Silencer, Dialog_silencer>(tableWidget, row, silencer_type);
        }
    }
}

void Widget_Silencer_inputTable::onInput()
{

}

void Widget_Silencer_inputTable::onOutput()
{

}

void Widget_Silencer_inputTable::on_pushButton_circle_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->page_circle);
    silencer_type = silencer_type_name::CIRCLE;
    ui->pushButton_circle->setStyleSheet("QPushButton { background-color: #E0EEF9; }");
    ui->pushButton_rect->setStyleSheet("QPushButton { background-color: white; }");
    ui->pushButton_circle_elbow->setStyleSheet("QPushButton { background-color: white; }");
    ui->pushButton_rect_elbow->setStyleSheet("QPushButton { background-color: white; }");
}


void Widget_Silencer_inputTable::on_pushButton_rect_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->page_rect);
    silencer_type = silencer_type_name::RECT;
    ui->pushButton_circle->setStyleSheet("QPushButton { background-color: white; }");
    ui->pushButton_rect->setStyleSheet("QPushButton { background-color: #E0EEF9; }");
    ui->pushButton_circle_elbow->setStyleSheet("QPushButton { background-color: white; }");
    ui->pushButton_rect_elbow->setStyleSheet("QPushButton { background-color: white; }");
}


void Widget_Silencer_inputTable::on_pushButton_circle_elbow_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->page_circle_elbow);
    silencer_type = silencer_type_name::CIRCLE_ELBOW;
    ui->pushButton_circle->setStyleSheet("QPushButton { background-color: white; }");
    ui->pushButton_rect->setStyleSheet("QPushButton { background-color: white; }");
    ui->pushButton_circle_elbow->setStyleSheet("QPushButton { background-color: #E0EEF9; }");
    ui->pushButton_rect_elbow->setStyleSheet("QPushButton { background-color: white; }");
}


void Widget_Silencer_inputTable::on_pushButton_rect_elbow_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->page_rect_elbow);
    silencer_type = silencer_type_name::RECT_ELBOW;
    ui->pushButton_circle->setStyleSheet("QPushButton { background-color: white; }");
    ui->pushButton_rect->setStyleSheet("QPushButton { background-color: white; }");
    ui->pushButton_circle_elbow->setStyleSheet("QPushButton { background-color: white; }");
    ui->pushButton_rect_elbow->setStyleSheet("QPushButton { background-color: #E0EEF9; }");
}

void Widget_Silencer_inputTable::clearTableFuc()
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

void Widget_Silencer_inputTable::loadComponentToTable()
{
    auto componentList = ComponentManager::getInstance().getComponentsByType(component_type_name::SILENCER);
    for (const auto& component : componentList) {
        if (auto silencerComponent = dynamic_cast<Circular_damper*>(component.data())) {
            auto lists = silencerComponent->getComponentDataAsStringList();
            QTableWidget* tableWidget = nullptr;
            if(silencerComponent->typeName() == silencer_type_name::CIRCLE)
                tableWidget = ui->tableWidget_circle;
            else if(silencerComponent->typeName() == silencer_type_name::RECT)
                tableWidget = ui->tableWidget_rect;
            else if(silencerComponent->typeName() == silencer_type_name::CIRCLE_ELBOW)
                tableWidget = ui->tableWidget_circle_elbow;
            else if(silencerComponent->typeName() == silencer_type_name::RECT_ELBOW)
                tableWidget = ui->tableWidget_rect_elbow;
            else
                continue;

            for (const auto& list : lists) {
                addRowToTable(tableWidget, list);
            }
        }
    }
}

