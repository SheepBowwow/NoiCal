#include "form_system_list.h"
#include "ui_form_system_list.h"
#include "dialog_add_system.h"

#include <QCheckBox>
#include <QMessageBox>

Form_system_list::Form_system_list(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form_system_list)
{
    ui->setupUi(this);
    ui->tableWidget->verticalHeader()->setVisible(false);

    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch); // 自适应大小
}

Form_system_list::~Form_system_list()
{
    delete ui;
}

void Form_system_list::on_buttonadd_clicked()
{
    // 获取选中的行索引
    int insertRow = -1;
    for (int row = 0; row < ui->tableWidget->rowCount(); ++row)
    {
        QWidget* widget = ui->tableWidget->cellWidget(row, 0); // Assuming the checkbox is in the first column (index 0)
        QCheckBox* checkBox = widget->findChild<QCheckBox*>(); // Find the checkbox within the widget
        if (checkBox && checkBox->isChecked())
        {
            insertRow=row+1;
            break;
        }
    }

    dialog_add_system *dialog= new dialog_add_system;

    if(dialog->exec()==QDialog::Accepted)
    {
        //设置插入行
        int RowCount=(insertRow!=-1)? insertRow : ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(RowCount);

        // 添加复选框
        QCheckBox* checkBox = new QCheckBox();
        QWidget* widget = new QWidget();
        widget->setStyleSheet("background-color: #C0C0C0;");
        QHBoxLayout* layout = new QHBoxLayout(widget);
        layout->addWidget(checkBox);
        layout->setAlignment(Qt::AlignCenter);
        layout->setContentsMargins(0, 0, 0, 0);
        ui->tableWidget->setCellWidget(RowCount, 0, widget);

        //添加内容
        QTableWidgetItem *tbitem1=new QTableWidgetItem(QString(dialog->getsyslx()));
        QTableWidgetItem *tbitem2=new QTableWidgetItem(QString(dialog->getsysbh()));
        QTableWidgetItem *tbitem3=new QTableWidgetItem(QString(dialog->getsysxh()));
        tbitem1->setFlags(Qt::ItemIsEditable); // 设置为只读
        tbitem1->setBackground(QBrush(Qt::lightGray)); // 只读单元格背景颜色设置为灰色
        tbitem2->setFlags(Qt::ItemIsEditable); // 设置为只读
        tbitem2->setBackground(QBrush(Qt::lightGray)); // 只读单元格背景颜色设置为灰色
        tbitem3->setFlags(Qt::ItemIsEditable); // 设置为只读
        tbitem3->setBackground(QBrush(Qt::lightGray)); // 只读单元格背景颜色设置为灰色

        ui->tableWidget->setItem(RowCount,1,tbitem1);
        ui->tableWidget->setItem(RowCount,2,tbitem2);
        ui->tableWidget->setItem(RowCount,3,tbitem3);

    }
}


void Form_system_list::on_buttondel_clicked()
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
    msgBox.exec();

    if (msgBox.clickedButton() == yesButton)
    {
        for (int row = ui->tableWidget->rowCount() - 1; row >= 0; --row)
        {
            QWidget *widget = ui->tableWidget->cellWidget(row, 0); // 提取出第一列的widget
            QCheckBox* checkBoxItem = widget->findChild<QCheckBox*>();          // widget转成checkbox
            if (checkBoxItem && checkBoxItem->checkState() == Qt::Checked)
            {
                ui->tableWidget->removeRow(row);
            }
        }
    }
}

void Form_system_list::on_buttonchange_clicked()
{
    // 获取选中的行索引
    int insertRow = -1;
    for (int row = 0; row < ui->tableWidget->rowCount(); ++row)
    {
        QWidget* widget = ui->tableWidget->cellWidget(row, 0); // Assuming the checkbox is in the first column (index 0)
        QCheckBox* checkBox = widget->findChild<QCheckBox*>(); // Find the checkbox within the widget
        if (checkBox && checkBox->isChecked())
        {
            insertRow=row;
            break;
        }
    }

    if(insertRow==-1)return;

    dialog_add_system *dialog = new dialog_add_system;

    QString s1 = ui->tableWidget->item(insertRow,1)->text();
    QString s2 = ui->tableWidget->item(insertRow,2)->text();
    QString s3 = ui->tableWidget->item(insertRow,3)->text();


    dialog->setvalues(0,s2,s3);

    if(dialog->exec()==QDialog::Accepted)
    {
        //添加内容
        QTableWidgetItem *tbitem1=new QTableWidgetItem(QString(dialog->getsyslx()));
        QTableWidgetItem *tbitem2=new QTableWidgetItem(QString(dialog->getsysbh()));
        QTableWidgetItem *tbitem3=new QTableWidgetItem(QString(dialog->getsysxh()));
        tbitem1->setFlags(Qt::ItemIsEditable); // 设置为只读
        tbitem1->setBackground(QBrush(Qt::lightGray)); // 只读单元格背景颜色设置为灰色
        tbitem2->setFlags(Qt::ItemIsEditable); // 设置为只读
        tbitem2->setBackground(QBrush(Qt::lightGray)); // 只读单元格背景颜色设置为灰色
        tbitem3->setFlags(Qt::ItemIsEditable); // 设置为只读
        tbitem3->setBackground(QBrush(Qt::lightGray)); // 只读单元格背景颜色设置为灰色

        ui->tableWidget->setItem(insertRow,1,tbitem1);
        ui->tableWidget->setItem(insertRow,2,tbitem2);
        ui->tableWidget->setItem(insertRow,3,tbitem3);

    }
}
