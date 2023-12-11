#include "form_room_define.h"
#include "ui_form_room_define.h"

#include <QCheckBox>


Form_room_define::Form_room_define(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form_room_define)
{
    ui->setupUi(this);
    ui->tableWidget_room_define->verticalHeader()->setVisible(false);
    int totalWidth = 0;
    for (int i = 0; i < ui->tableWidget_room_define->columnCount(); ++i)
    {
        totalWidth += ui->tableWidget_room_define->columnWidth(i);
    }
    for (int i = 0; i < ui->tableWidget_room_define->columnCount(); ++i)
    {
        double ratio = static_cast<double>(ui->tableWidget_room_define->columnWidth(i)) / totalWidth;
        int columnWidth = static_cast<int>(ratio * ui->tableWidget_room_define->width());
        ui->tableWidget_room_define->setColumnWidth(i, columnWidth);
    }
    ui->tableWidget_room_define->horizontalHeader()->setStretchLastSection(true);
    ui->tableWidget_room_define->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    this->buttonToHeader(ui->tableWidget_room_define,ui->buttonWidget_room,SLOT(addroom()),SLOT(delroom()));

    dialog = new Dialog_addroom;

    //弹窗点击确定，接收信号并发送提醒主界面改变
    connect(dialog,SIGNAL(dialogsent(QString,int)),this,SLOT(jieshou(QString,int)));

}

Form_room_define::~Form_room_define()
{
    delete ui;
}

void Form_room_define::setjiabanItem(QTreeWidgetItem *item)
{
    _item=item;
}

void Form_room_define::buttonToHeader(QTableWidget *tableWidget, QWidget *buttonWidget, const char *addButtonSlot, const char *delButtonSlot)
{
    QHeaderView *header = tableWidget->horizontalHeader();
    int lastColumnIndex = header->count() - 1;

    int x = header->sectionViewportPosition(lastColumnIndex);
    int y = 0;
    int width = header->sectionSize(lastColumnIndex);
    int height = header->height();

    // 将相对坐标映射到当前页的坐标系
    QPoint relativePos = header->mapTo(tableWidget->parentWidget(), QPoint(x, y));

    // 设置按钮的几何位置
    buttonWidget->setGeometry(
        relativePos.x() + 1,
        relativePos.y(),
        width - 2,
        height - 2
    );

    QHBoxLayout* layout = new QHBoxLayout(buttonWidget);
    // 创建"+"按钮
    QPushButton* addButton = new QPushButton("+");
    addButton->setFixedSize(15, 15);  // 设置按钮的尺寸为正方形，例如30x30像素
    layout->addWidget(addButton);
    // 创建"-"按钮
    QPushButton* delButton = new QPushButton("-");
    delButton->setFixedSize(15, 15);  // 设置按钮的尺寸为正方形，例如30x30像素
    layout->addWidget(delButton);
    // 在构造函数或者初始化函数中连接信号和槽函数
    connect(addButton, SIGNAL(clicked()), this, addButtonSlot);
    connect(delButton, SIGNAL(clicked()), this, delButtonSlot);

    buttonWidget->setStyleSheet(QString("#%1{background : rgb(157, 198, 230);}").arg(buttonWidget->objectName()));
}

void Form_room_define::addroom() // 通过弹窗添加房间
{

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
        // 添加加减按钮
        QWidget* widget2 = new QWidget();
        QHBoxLayout* layout2 = new QHBoxLayout(widget2);
        QPushButton* addButton = new QPushButton("+");
        addButton->setFixedSize(15, 15);
        layout2->addWidget(addButton);
        QPushButton* delButton = new QPushButton("-");
        delButton->setFixedSize(15, 15);
        layout2->addWidget(delButton);
        ui->tableWidget_room_define->setCellWidget(RowCount, 8, widget2);
        // 在构造函数或者初始化函数中连接信号和槽函数
        //connect(addButton, SIGNAL(clicked()), this, addButtonSlot);
        //connect(delButton, SIGNAL(clicked()), this, delButtonSlot);

        //添加内容
        QTableWidgetItem *tbitem1=new QTableWidgetItem(QString(dialog->getRoomid()));
        QTableWidgetItem *tbitem2=new QTableWidgetItem(QString(dialog->getroomclass()));
        QTableWidgetItem *tbitem5=new QTableWidgetItem(QString::number(dialog->getmainpipe()));
        QTableWidgetItem *tbitem6=new QTableWidgetItem(QString(dialog->getlimit()));
        QTableWidgetItem *tbitem7=new QTableWidgetItem(QString(dialog->getroomcalclass()));
        tbitem1->setFlags(Qt::ItemIsEditable); // 设置为只读
        tbitem1->setBackground(QBrush(Qt::lightGray)); // 只读单元格背景颜色设置为灰色
        tbitem2->setFlags(Qt::ItemIsEditable); // 设置为只读
        tbitem2->setBackground(QBrush(Qt::lightGray)); // 只读单元格背景颜色设置为灰色
        tbitem5->setFlags(Qt::ItemIsEditable); // 设置为只读
        tbitem5->setBackground(QBrush(Qt::lightGray)); // 只读单元格背景颜色设置为灰色
        tbitem6->setFlags(Qt::ItemIsEditable); // 设置为只读
        tbitem6->setBackground(QBrush(Qt::lightGray)); // 只读单元格背景颜色设置为灰色
        tbitem7->setFlags(Qt::ItemIsEditable); // 设置为只读
        tbitem7->setBackground(QBrush(Qt::lightGray)); // 只读单元格背景颜色设置为灰色

        ui->tableWidget_room_define->setItem(RowCount,1,tbitem1);
        ui->tableWidget_room_define->setItem(RowCount,2,tbitem2);
        ui->tableWidget_room_define->setItem(RowCount,5,tbitem5);
        ui->tableWidget_room_define->setItem(RowCount,6,tbitem6);
        ui->tableWidget_room_define->setItem(RowCount,7,tbitem7);

    }

}

void Form_room_define::delroom()
{

}

void Form_room_define::jieshou(QString name,int num)
{
    emit formsent(_item,name,num);
}
