#include "form_room_define.h"
#include "ui_form_room_define.h"

#include <QCheckBox>
#include <QMessageBox>


Form_room_define::Form_room_define(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form_room_define)
{
    ui->setupUi(this);
    ui->tableWidget_room_define->verticalHeader()->setVisible(false);

    ui->tableWidget_room_define->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

}

Form_room_define::~Form_room_define()
{
    delete ui;
}

void Form_room_define::setjiabanItem(QTreeWidgetItem *item)
{
    _jiabanitem=item;
}

void Form_room_define::jieshou(QString name,int num, QString jiaban, QString limit)
{
    emit roomadd(_jiabanitem,name,num,jiaban,limit);
}

void Form_room_define::on_buttonadd_clicked()
{
    // 获取选中的行索引
    int insertRow = -1;
    for (int row = 0; row < ui->tableWidget_room_define->rowCount(); ++row)
    {
        QWidget* widget = ui->tableWidget_room_define->cellWidget(row, 0); // Assuming the checkbox is in the first column (index 0)
        QCheckBox* checkBox = widget->findChild<QCheckBox*>(); // Find the checkbox within the widget
        if (checkBox && checkBox->isChecked())
        {
            insertRow=row+1;
            break;
        }
    }

    dialog = new Dialog_addroom(this);

    //弹窗点击确定，接收信号并发送提醒主界面改变
    connect(dialog,SIGNAL(dialogsent(QString,int,QString, QString)),this,SLOT(jieshou(QString,int,QString, QString)));

    if(dialog->exec()==QDialog::Accepted)
    {
        //设置插入行
        int RowCount=(insertRow!=-1)? insertRow : ui->tableWidget_room_define->rowCount();
        ui->tableWidget_room_define->insertRow(RowCount);

        // 添加复选框
        QCheckBox* checkBox = new QCheckBox();
        QWidget* widget = new QWidget();
        widget->setStyleSheet("background-color: #C0C0C0;");
        QHBoxLayout* layout = new QHBoxLayout(widget);
        layout->addWidget(checkBox);
        layout->setAlignment(Qt::AlignCenter);
        layout->setContentsMargins(0, 0, 0, 0);
        ui->tableWidget_room_define->setCellWidget(RowCount, 0, widget);

        //添加内容
        QTableWidgetItem *tbitem1=new QTableWidgetItem(QString(dialog->getroomid()));
        QTableWidgetItem *tbitem2=new QTableWidgetItem(QString(dialog->getroomname()));
        QTableWidgetItem *tbitem3=new QTableWidgetItem(QString(dialog->getroomclass()));
        QTableWidgetItem *tbitem4=new QTableWidgetItem(dialog->getjiaban());
        QTableWidgetItem *tbitem5=new QTableWidgetItem(QString::number(dialog->getmainpipe()));
        QTableWidgetItem *tbitem6=new QTableWidgetItem(QString(dialog->getlimit()));
        QTableWidgetItem *tbitem7=new QTableWidgetItem(QString(dialog->getroomcalclass()));
        tbitem1->setFlags(Qt::ItemIsEditable); // 设置为只读
        tbitem1->setBackground(QBrush(Qt::lightGray)); // 只读单元格背景颜色设置为灰色
        tbitem2->setFlags(Qt::ItemIsEditable); // 设置为只读
        tbitem2->setBackground(QBrush(Qt::lightGray)); // 只读单元格背景颜色设置为灰色
        tbitem3->setFlags(Qt::ItemIsEditable); // 设置为只读
        tbitem3->setBackground(QBrush(Qt::lightGray)); // 只读单元格背景颜色设置为灰色
        tbitem4->setFlags(Qt::ItemIsEditable); // 设置为只读
        tbitem4->setBackground(QBrush(Qt::lightGray)); // 只读单元格背景颜色设置为灰色
        tbitem5->setFlags(Qt::ItemIsEditable); // 设置为只读
        tbitem5->setBackground(QBrush(Qt::lightGray)); // 只读单元格背景颜色设置为灰色
        tbitem6->setFlags(Qt::ItemIsEditable); // 设置为只读
        tbitem6->setBackground(QBrush(Qt::lightGray)); // 只读单元格背景颜色设置为灰色
        tbitem7->setFlags(Qt::ItemIsEditable); // 设置为只读
        tbitem7->setBackground(QBrush(Qt::lightGray)); // 只读单元格背景颜色设置为灰色

        ui->tableWidget_room_define->setItem(RowCount,1,tbitem1);
        ui->tableWidget_room_define->setItem(RowCount,2,tbitem2);
        ui->tableWidget_room_define->setItem(RowCount,3,tbitem3);
        ui->tableWidget_room_define->setItem(RowCount,4,tbitem4);
        ui->tableWidget_room_define->setItem(RowCount,5,tbitem5);
        ui->tableWidget_room_define->setItem(RowCount,6,tbitem6);
        ui->tableWidget_room_define->setItem(RowCount,7,tbitem7);

    }
}
void Form_room_define::on_buttondel_clicked()
{
    // 获取选中的行索引
    QList<int> selectedRows;
    for (int row = 0; row < ui->tableWidget_room_define->rowCount(); ++row)
    {
        QWidget* widget = ui->tableWidget_room_define->cellWidget(row, 0); // Assuming the checkbox is in the first column (index 0)
        QCheckBox* checkBox = widget->findChild<QCheckBox*>(); // Find the checkbox within the widget

        if (checkBox && checkBox->isChecked())
        {
            selectedRows.append(row);
        }
    }

    // 弹窗确认
    QString confirmationMessage = "确认删除所选房间吗？\n";

    QMessageBox msgBox;
    msgBox.setWindowTitle("确认删除");
    msgBox.setText(confirmationMessage);
    msgBox.setIcon(QMessageBox::Warning);
    QPushButton *yesButton = msgBox.addButton("确认", QMessageBox::YesRole);
    QPushButton *noButton = msgBox.addButton("取消", QMessageBox::NoRole);
    msgBox.exec();

    if (msgBox.clickedButton() == yesButton)
    {
        for (int row = ui->tableWidget_room_define->rowCount() - 1; row >= 0; --row)
        {
            QWidget *widget = ui->tableWidget_room_define->cellWidget(row, 0); // 提取出第一列的widget
            QCheckBox* checkBoxItem = widget->findChild<QCheckBox*>();          // widget转成checkbox
            if (checkBoxItem && checkBoxItem->checkState() == Qt::Checked)
            {
                emit roomdel(_jiabanitem,ui->tableWidget_room_define->item(row,1)->text());
                ui->tableWidget_room_define->removeRow(row);
            }
        }
    }
}

void Form_room_define::on_buttonchange_clicked()
{
    // 获取选中的行索引
    int insertRow = -1;
    for (int row = 0; row < ui->tableWidget_room_define->rowCount(); ++row)
    {
        QWidget* widget = ui->tableWidget_room_define->cellWidget(row, 0); // Assuming the checkbox is in the first column (index 0)
        QCheckBox* checkBox = widget->findChild<QCheckBox*>(); // Find the checkbox within the widget
        if (checkBox && checkBox->isChecked())
        {
            insertRow=row;
            break;
        }
    }

    if(insertRow==-1)return;

    dialog = new Dialog_addroom;

    QString s1 = ui->tableWidget_room_define->item(insertRow,1)->text();
    QString s2 = ui->tableWidget_room_define->item(insertRow,2)->text();
    QString s3 = ui->tableWidget_room_define->item(insertRow,4)->text();
    QString num = ui->tableWidget_room_define->item(insertRow,5)->text();
    QString s4 = ui->tableWidget_room_define->item(insertRow,6)->text();
    QString s5 = ui->tableWidget_room_define->item(insertRow,7)->text();


    dialog->setvalues(s1,s2,s3,num,s4,s5);

    if(dialog->exec()==QDialog::Accepted)
    {
        //添加内容
        QTableWidgetItem *tbitem1=new QTableWidgetItem(QString(dialog->getroomid()));
        QTableWidgetItem *tbitem2=new QTableWidgetItem(QString(dialog->getroomname()));
        QTableWidgetItem *tbitem3=new QTableWidgetItem(QString(dialog->getroomclass()));
        QTableWidgetItem *tbitem4=new QTableWidgetItem(dialog->getjiaban());
        QTableWidgetItem *tbitem5=new QTableWidgetItem(QString::number(dialog->getmainpipe()));
        QTableWidgetItem *tbitem6=new QTableWidgetItem(QString(dialog->getlimit()));
        QTableWidgetItem *tbitem7=new QTableWidgetItem(QString(dialog->getroomcalclass()));
        tbitem1->setFlags(Qt::ItemIsEditable); // 设置为只读
        tbitem1->setBackground(QBrush(Qt::lightGray)); // 只读单元格背景颜色设置为灰色
        tbitem2->setFlags(Qt::ItemIsEditable); // 设置为只读
        tbitem2->setBackground(QBrush(Qt::lightGray)); // 只读单元格背景颜色设置为灰色
        tbitem3->setFlags(Qt::ItemIsEditable); // 设置为只读
        tbitem3->setBackground(QBrush(Qt::lightGray)); // 只读单元格背景颜色设置为灰色
        tbitem4->setFlags(Qt::ItemIsEditable); // 设置为只读
        tbitem4->setBackground(QBrush(Qt::lightGray)); // 只读单元格背景颜色设置为灰色
        tbitem5->setFlags(Qt::ItemIsEditable); // 设置为只读
        tbitem5->setBackground(QBrush(Qt::lightGray)); // 只读单元格背景颜色设置为灰色
        tbitem6->setFlags(Qt::ItemIsEditable); // 设置为只读
        tbitem6->setBackground(QBrush(Qt::lightGray)); // 只读单元格背景颜色设置为灰色
        tbitem7->setFlags(Qt::ItemIsEditable); // 设置为只读
        tbitem7->setBackground(QBrush(Qt::lightGray)); // 只读单元格背景颜色设置为灰色

        ui->tableWidget_room_define->setItem(insertRow,1,tbitem1);
        ui->tableWidget_room_define->setItem(insertRow,2,tbitem2);
        ui->tableWidget_room_define->setItem(insertRow,3,tbitem3);
        ui->tableWidget_room_define->setItem(insertRow,4,tbitem4);
        ui->tableWidget_room_define->setItem(insertRow,5,tbitem5);
        ui->tableWidget_room_define->setItem(insertRow,6,tbitem6);
        ui->tableWidget_room_define->setItem(insertRow,7,tbitem7);

    }
}
