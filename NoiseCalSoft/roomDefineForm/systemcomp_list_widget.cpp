#include "systemcomp_list_widget.h"
#include "ui_systemcomp_list_widget.h"
#include "dialog_add_system.h"
#include "globle_var.h"
#include "roomDefineForm/roomcalinfomanager.h"

#include <QCheckBox>
#include <QMessageBox>

SystemComp_list_widget::SystemComp_list_widget(QString system_name, QWidget *parent) :
    QWidget(parent),
    system_name(system_name),
    ui(new Ui::SystemComp_list_widget)
{
    ui->setupUi(this);
    ui->tableWidget->verticalHeader()->setVisible(false);

    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch); // 自适应大小

    // 连接 RoomCalInfoManager 的信号和槽函数
    connect(&RoomCalInfoManager::getInstance(), &RoomCalInfoManager::compAddToSystemList,
            this, &SystemComp_list_widget::addCompToTable);
}

SystemComp_list_widget::~SystemComp_list_widget()
{
    delete ui;
}

void SystemComp_list_widget::addCompToTable(const QString &systemName, const QString &type, const QString &number, const QString &model, const QString &componentUUID, int& table_index)
{
    if(systemName != this->system_name)
        return;

    // 获取选中的行索引
    int targetRow = -1;
    for (int row = 0; row < ui->tableWidget->rowCount(); ++row)
    {
        QWidget* cellWidget = ui->tableWidget->cellWidget(row, 0); // 假设复选框在第一列（索引为0）
        QCheckBox* checkBox = cellWidget->findChild<QCheckBox*>(); // 在小部件内查找复选框
        if (checkBox && checkBox->isChecked())
        {
            targetRow = row + 1;
            break;
        }
    }

    // 设置插入行
    int rowToInsert = (targetRow != -1) ? targetRow : ui->tableWidget->rowCount();
    table_index = rowToInsert;
    ui->tableWidget->insertRow(rowToInsert);

    // 添加复选框
    QCheckBox* newCheckBox = new QCheckBox();
    QWidget* checkBoxWidget = new QWidget();
    checkBoxWidget->setStyleSheet("background-color: #C0C0C0;");
    QHBoxLayout* layout = new QHBoxLayout(checkBoxWidget);
    layout->addWidget(newCheckBox);
    layout->setAlignment(Qt::AlignCenter);
    layout->setContentsMargins(0, 0, 0, 0);
    ui->tableWidget->setCellWidget(rowToInsert, 0, checkBoxWidget);

    // 添加内容
    QTableWidgetItem *typeItem = new QTableWidgetItem(type);
    QTableWidgetItem *numberItem = new QTableWidgetItem(number.trimmed().isEmpty() ? "-" : number);
    QTableWidgetItem *modelItem = new QTableWidgetItem(model);
    typeItem->setFlags(Qt::ItemIsEditable); // 设置为只读
    typeItem->setBackground(QBrush(Qt::lightGray)); // 只读单元格背景颜色设置为灰色
    numberItem->setFlags(Qt::ItemIsEditable); // 设置为只读
    numberItem->setBackground(QBrush(Qt::lightGray)); // 只读单元格背景颜色设置为灰色
    modelItem->setFlags(Qt::ItemIsEditable); // 设置为只读
    modelItem->setBackground(QBrush(Qt::lightGray)); // 只读单元格背景颜色设置为灰色

    ui->tableWidget->setItem(rowToInsert, 1, typeItem);
    ui->tableWidget->setItem(rowToInsert, 2, numberItem);
    ui->tableWidget->setItem(rowToInsert, 3, modelItem);
}


void SystemComp_list_widget::on_pushButton_add_clicked()
{
    dialog_add_system *dialog = new dialog_add_system;

    if (dialog->exec() == QDialog::Accepted)
    {
        QString type = dialog->getType();
        QString number = dialog->getNumber();
        QString model = dialog->getModel();

        RoomCalInfoManager::getInstance().addCompToSystemList(this->system_name, type, number, model);
    }
}


void SystemComp_list_widget::on_pushButton_del_clicked()
{
    // 获取选中的行索引
    QList<int> selectedRows;
    for (int row = 0; row < ui->tableWidget->rowCount(); ++row)
    {
        QWidget* widget = ui->tableWidget->cellWidget(row, 0); // Assuming the checkbox is in the first column (index 0)
        QCheckBox* checkBox = widget->findChild<QCheckBox*>(); // Find the checkbox within the widget

        if (checkBox && checkBox->isChecked())
        {
            selectedRows.append(row);
        }
    }

    // 弹窗确认
    QString confirmationMessage = "确认删除所选设备吗？\n";

    QMessageBox msgBox;
    msgBox.setWindowTitle("确认删除");
    msgBox.setText(confirmationMessage);
    msgBox.setIcon(QMessageBox::Warning);
    QPushButton *yesButton = msgBox.addButton("确认", QMessageBox::YesRole);
    QPushButton *noButton = msgBox.addButton("取消", QMessageBox::NoRole);
    Q_UNUSED(noButton)
    msgBox.exec();

    if (msgBox.clickedButton() == yesButton)
    {
        for (int row = ui->tableWidget->rowCount() - 1; row >= 0; --row)
        {
            QWidget *widget = ui->tableWidget->cellWidget(row, 0); // 提取出第一列的widget

            QCheckBox* checkBoxItem = widget->findChild<QCheckBox*>();          // widget转成checkbox
            if (checkBoxItem && checkBoxItem->checkState() == Qt::Checked)
            {
                QString type = ui->tableWidget->item(row, 1)->text();
                QString number = ui->tableWidget->item(row, 2)->text();
                QString model = ui->tableWidget->item(row, 3)->text();
                RoomCalInfoManager::getInstance().deleteCompInSystemList(this->system_name, type, number, model);
                ui->tableWidget->removeRow(row);
            }
        }
    }
}

void SystemComp_list_widget::on_pushButton_revise_clicked()
{
    // 获取选中的行索引
    int rowToInsert = -1;
    for (int row = 0; row < ui->tableWidget->rowCount(); ++row)
    {
        QWidget* widget = ui->tableWidget->cellWidget(row, 0); // Assuming the checkbox is in the first column (index 0)
        QCheckBox* checkBox = widget->findChild<QCheckBox*>(); // Find the checkbox within the widget
        if (checkBox && checkBox->isChecked())
        {
            rowToInsert = row;
            break;
        }
    }

    if(rowToInsert == -1) return;

    dialog_add_system *dialog = new dialog_add_system;

    QString old_type = ui->tableWidget->item(rowToInsert, 1)->text();
    QString old_number = ui->tableWidget->item(rowToInsert, 2)->text();
    QString old_model = ui->tableWidget->item(rowToInsert, 3)->text();

    dialog->setvalues(old_type, old_number, old_model);

    if(dialog->exec()==QDialog::Accepted)
    {
        QString new_type = dialog->getType();
        QString new_number = dialog->getNumber();
        QString new_model = dialog->getModel();

        // 添加内容
        QTableWidgetItem *typeItem = new QTableWidgetItem(new_type);
        QTableWidgetItem *numberItem = new QTableWidgetItem(new_number);
        QTableWidgetItem *modelItem = new QTableWidgetItem(new_model);
        typeItem->setFlags(Qt::ItemIsEditable); // 设置为只读
        typeItem->setBackground(QBrush(Qt::lightGray)); // 只读单元格背景颜色设置为灰色
        numberItem->setFlags(Qt::ItemIsEditable); // 设置为只读
        numberItem->setBackground(QBrush(Qt::lightGray)); // 只读单元格背景颜色设置为灰色
        modelItem->setFlags(Qt::ItemIsEditable); // 设置为只读
        modelItem->setBackground(QBrush(Qt::lightGray)); // 只读单元格背景颜色设置为灰色

        ui->tableWidget->setItem(rowToInsert, 1, typeItem);
        ui->tableWidget->setItem(rowToInsert, 2, numberItem);
        ui->tableWidget->setItem(rowToInsert, 3, modelItem);

        RoomCalInfoManager::getInstance().updateCompInSystemList(this->system_name, old_type, old_number, old_model,
                                                                 new_type, new_number, new_model);
    }
}
