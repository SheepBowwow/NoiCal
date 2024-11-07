#include "room_cal_total_widget.h"
#include "ui_room_cal_total_widget.h"

Room_cal_total_widget::Room_cal_total_widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Room_cal_total_widget)
{
    ui->setupUi(this);
    initTableWidget();
}

Room_cal_total_widget::~Room_cal_total_widget()
{
    delete ui;
}

void Room_cal_total_widget::change_outer_cal()
{
    ui->label_cg1->setText("室外编号:");
    ui->label_cg2->setText("噪音源支管数量:");
    ui->label_cg3->setText("室外噪音总值dB(A):");
    // 设置列标题
    ui->tableWidget->setColumnCount(3);
    QStringList headerLabels;
    headerLabels<<"室外编号"<<"测试点距离(m)"<<"A级计权噪音dB(A)";
    ui->tableWidget->setHorizontalHeaderLabels(headerLabels);
}

void Room_cal_total_widget::setInfo(QString MVZName, QString deck,QString numer, QString noiseLimit, QString ductNum)
{
    ui->lineEdit_MVZName->setText(MVZName);
    ui->lineEdit_deck->setText(deck);
    ui->lineEdit_room_number->setText(numer);
    ui->lineEdit_noise_limit->setText(noiseLimit);
    ui->lineEdit_duct_num->setText(ductNum);
}

void Room_cal_total_widget::setDuctNum(QString ductNum)
{
    ui->lineEdit_duct_num->setText(ductNum);
}

void Room_cal_total_widget::setMVZName(QString MVZName)
{
    ui->lineEdit_MVZName->setText(MVZName);
}

///TODO 如果有主风管删除，主风管改名则要一并修改这里

/*
 * 给表格数据按照treeWidget排序
// 假设 treeWidget 是你的 QTreeWidget 对象
QList<QTreeWidgetItem*> treeItems = treeWidget->findItems("", Qt::MatchContains);
QMap<QString, int> orderMap;

// 根据 treeWidget 的顺序创建一个映射
for (int i = 0; i < treeItems.size(); ++i) {
    QString ductNumber = treeItems[i]->text(0); // 假设 ductNumber 在第一列
    orderMap.insert(ductNumber, i);
}

// 重新排序 tableWidget 中的行
QList<QTableWidgetItem*> tableItems;
for (int row = 0; row < ui->tableWidget->rowCount(); ++row) {
    QString ductNumber = ui->tableWidget->item(row, 0)->text(); // 假设 ductNumber 在第一列
    tableItems.append(ui->tableWidget->takeItem(row, 0)); // 移除并获取项
}

// 根据 orderMap 中的顺序插入项
qSort(tableItems.begin(), tableItems.end(), [&](QTableWidgetItem* a, QTableWidgetItem* b) {
    return orderMap.value(a->text()) < orderMap.value(b->text());
});

// 清空表格并重新插入已排序的项
ui->tableWidget->clear();
for (int row = 0; row < tableItems.size(); ++row) {
    ui->tableWidget->insertRow(row);
    ui->tableWidget->setItem(row, 0, tableItems[row]); // 插入 ductNumber
    // 继续插入其他列的数据
    // 根据你的数据结构，可能需要保存其他列的数据，类似于之前的代码
}
说明
创建映射：我们遍历 QTreeWidget 的项，将每个 ductNumber 的顺序存储在一个 QMap 中。
移除并获取项：我们从 QTableWidget 中移除每一项，并将其存储在 tableItems 列表中。
排序：使用 qSort 对 tableItems 进行排序，依据 orderMap 中的顺序。
清空并重新插入：清空 QTableWidget，并按照新的顺序插入已排序的项。
 */
//主风管改名就发送原本的编号和新编号进行替换，主风管删除就发送被删除的编号删除行
void Room_cal_total_widget::addDuctToTable(QString ductNumber, double test_point_distance, double A_widget_noise)
{
    QList<QTableWidgetItem*> items = ui->tableWidget->findItems(ductNumber, Qt::MatchExactly);
    bool found = false;

    for (QTableWidgetItem* item : items) {
        if (item->column() == 0) { // 第一列
            found = true;
            // 更新已找到的行，可以选择更新其他列的数据
            int row = item->row();
            ui->tableWidget->item(row, 1)->setText(QString::number(test_point_distance));
            ui->tableWidget->item(row, 2)->setText(QString::number(A_widget_noise));
            break;
        }
    }

    // 如果主风管不在表格里
    if (!found) {
        int rowCount = ui->tableWidget->rowCount();
        ui->tableWidget->setRowCount(rowCount + 1); // 添加新行到末尾

        // 插入数据项
        QVector<QString> data = { ductNumber, QString::number(test_point_distance), QString::number(A_widget_noise) };

        for (int i = 0; i < data.size(); ++i) {
            QTableWidgetItem *item = new QTableWidgetItem(data[i]);
            ui->tableWidget->setItem(rowCount, i, item);
            item->setTextAlignment(Qt::AlignCenter); // 内容居中对齐
            item->setFlags(item->flags() & ~Qt::ItemIsEditable); // 设置为只读
            item->setBackground(QBrush(Qt::lightGray)); // 背景颜色灰色
            item->setData(Qt::ForegroundRole, QColor(70,70,70)); // 文本颜色深灰色
        }
    }
}

void Room_cal_total_widget::handle_duct_number_revise(QString origin_number, QString new_number)
{
    QList<QTableWidgetItem*> items = ui->tableWidget->findItems(origin_number, Qt::MatchExactly);

    for (QTableWidgetItem* item : items) {
        if (item->column() == 0) { // 第一列
            int row = item->row();
            // 更新第一列的管道编号
            ui->tableWidget->item(row, 0)->setText(new_number);
            break; // 找到并更新后跳出循环
        }
    }
}

void Room_cal_total_widget::handle_duct_remove(QString remove_number)
{
    QList<QTableWidgetItem*> items = ui->tableWidget->findItems(remove_number, Qt::MatchExactly);

    for (QTableWidgetItem* item : items) {
        if (item->column() == 0) { // 第一列
            int row = item->row();
            ui->tableWidget->removeRow(row); // 删除找到的行
            break; // 找到并删除后跳出循环
        }
    }
}

void Room_cal_total_widget::initTableWidget()
{
    ui->tableWidget->setRowCount(0);
    ui->tableWidget->verticalHeader()->setVisible(false);
    ui->tableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    ui->tableWidget->setFocusPolicy(Qt::NoFocus);
    int totalWidth = 0;

    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // 强制更新表格布局
    ui->tableWidget->update();
}
