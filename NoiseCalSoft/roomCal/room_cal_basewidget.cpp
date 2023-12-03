#include "room_cal_basewidget.h"
#include "ui_room_cal_basewidget.h"
#include "roomCal/roomInfoTable/table_noi_src.h"
#include <QVBoxLayout>

room_cal_baseWidget::room_cal_baseWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::room_cal_baseWidget)
{
    ui->setupUi(this);
    QWidget *scrollWidget = new QWidget;
    QVBoxLayout *scrollLayout = new QVBoxLayout(ui->scrollArea);
    scrollLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);

//    // 设置垂直方向上的间距为10像素
    scrollLayout->setSpacing(30);
    scrollLayout->setContentsMargins(0, 15, 0, 0);
    Table_noi_src* table1 = new Table_noi_src;
    scrollLayout->addWidget(table1);
    Table_noi_src* table2 = new Table_noi_src;
    scrollLayout->addWidget(table2);

    Table_noi_src* table3 = new Table_noi_src;
    scrollLayout->addWidget(table3);
    Table_noi_src* table4 = new Table_noi_src;
    scrollLayout->addWidget(table4);
    Table_noi_src* table5 = new Table_noi_src;
    scrollLayout->addWidget(table5);


    ui->scrollArea->setWidgetResizable(true);
    scrollWidget->setLayout(scrollLayout);
    ui->scrollArea->setWidget(scrollWidget);

}

room_cal_baseWidget::~room_cal_baseWidget()
{
    delete ui;
}
