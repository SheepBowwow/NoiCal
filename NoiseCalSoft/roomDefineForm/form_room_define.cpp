#include "form_room_define.h"
#include "ui_form_room_define.h"

#include <QCheckBox>


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

void Form_room_define::jieshou(QString name,int num)
{
    emit roomadd(_jiabanitem,name,num);
}

void Form_room_define::on_buttonadd_clicked()
{
    dialog = new Dialog_addroom;

    //弹窗点击确定，接收信号并发送提醒主界面改变
    connect(dialog,SIGNAL(dialogsent(QString,int)),this,SLOT(jieshou(QString,int)));

    if(dialog->exec()==QDialog::Accepted)
    {
        int RowCount = ui->tableWidget_room_define->rowCount();
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
        QTableWidgetItem *tbitem4=new QTableWidgetItem(_jiabanitem->parent()->text(0));
        QTableWidgetItem *tbitem5=new QTableWidgetItem(_jiabanitem->text(0));
        QTableWidgetItem *tbitem6=new QTableWidgetItem(QString::number(dialog->getmainpipe()));
        QTableWidgetItem *tbitem7=new QTableWidgetItem(QString(dialog->getlimit()));
        QTableWidgetItem *tbitem8=new QTableWidgetItem(QString(dialog->getroomcalclass()));
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
        tbitem8->setFlags(Qt::ItemIsEditable); // 设置为只读
        tbitem8->setBackground(QBrush(Qt::lightGray)); // 只读单元格背景颜色设置为灰色

        ui->tableWidget_room_define->setItem(RowCount,1,tbitem1);
        ui->tableWidget_room_define->setItem(RowCount,2,tbitem2);
        ui->tableWidget_room_define->setItem(RowCount,3,tbitem3);
        ui->tableWidget_room_define->setItem(RowCount,4,tbitem4);
        ui->tableWidget_room_define->setItem(RowCount,5,tbitem5);
        ui->tableWidget_room_define->setItem(RowCount,6,tbitem6);
        ui->tableWidget_room_define->setItem(RowCount,7,tbitem7);
        ui->tableWidget_room_define->setItem(RowCount,8,tbitem8);

    }
}
void Form_room_define::on_buttondel_clicked()
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
