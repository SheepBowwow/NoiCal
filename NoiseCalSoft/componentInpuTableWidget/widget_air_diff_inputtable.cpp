#include "widget_air_diff_inputtable.h"
#include "ui_widget_base_inputtable.h"
#include "inputDialog/dialog_air_diff.h"
#include <QComboBox>
#include <QRadioButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QSpacerItem>
#include <QListView>
#include <QDebug>

Widget_air_diff_inputTable::Widget_air_diff_inputTable(bool inComponentDB, QWidget *parent)
    : Widget_base_inputTable(inComponentDB, parent)
{
    setTitle("布风器+散流器");
    mergeRowCount = 3;
    //连接槽函数
    connect(ui->pushButton_add, &QPushButton::clicked, this, &Widget_air_diff_inputTable::onAdd);
    connect(ui->pushButton_del, &QPushButton::clicked, this, &Widget_air_diff_inputTable::onDel);
    connect(ui->pushButton_revise, &QPushButton::clicked, this, &Widget_air_diff_inputTable::onRevise);
    connect(ui->pushButton_input, &QPushButton::clicked, this, &Widget_air_diff_inputTable::onInput);
    connect(ui->pushButton_output, &QPushButton::clicked, this, &Widget_air_diff_inputTable::onOutput);
    //connect(ui->pushButton_filter, &QPushButton::clicked, this, &Widget_air_diff_inputTable::onFilter);
}

void Widget_air_diff_inputTable::initTableWidget()
{
    colCount = 20;
    headerText<< "" << "序号" << "布风器型号" << "布风器品牌" << "散流器型号" << "散流器品牌" << "末端\n类型" << "末端\n尺寸"
               << "数据类型" << "63Hz\n(dB)" << "125Hz\n(dB)" << "250Hz\n(dB)" << "500Hz\n(dB)" << "1kHz\n(dB)" << "2kHz\n(dB)"
               << "4kHz\n(dB)" << "8kHz\n(dB)" << "总值\ndB(A)" << "来源" << "UUID";  //表头标题用QStringList来表示
    QVector<int> widths = {30, 40, 120, 100, 120, 100, 50, 70, 80, 55, 55, 55, 55, 55, 55, 55, 55, 55, 65, 0};
    columnWidths = widths;
    // 调用封装好的初始化表格函数
    setTableWidget(ui->tableWidget, headerText, columnWidths, colCount);
    QStringList merge
        {
            "",
            "序号",
            "布风器型号",
            "布风器品牌",
            "散流器型号",
            "散流器品牌",
            "末端\n类型",
            "静压\n(Pa)",
            "末端\n尺寸",
            "UUID"
        };
    this->mergeCols = merge;
    // 隐藏最后一列
    ui->tableWidget->setColumnHidden(colCount - 1, true);
}

void Widget_air_diff_inputTable::onAdd()
{
    if(inComponentDB)
    {
        QTableWidget *tableWidget = ui->tableWidget;
        Dialog_air_diff *dialog = new Dialog_air_diff(nullptr);
        QSharedPointer<AirDiff> component = nullptr;

        if (dialog->exec() == QDialog::Accepted) {
            if(AirDiff* rawPointer = static_cast<AirDiff*>(dialog->getComponent()))
                component = QSharedPointer<AirDiff>(rawPointer);
            else
                return;
            component->table_id = QString::number(tableWidget->rowCount() / 3 + 1);
            if (component != nullptr) {
                auto lists = component->getComponentDataAsStringList(inComponentDB);
                // 使用通用函数添加行
                addRowToTable(tableWidget, lists[0]);
                addRowToTable(tableWidget, lists[1]);
                addRowToTable(tableWidget, lists[2]);

                mergeColumnsByNames(tableWidget, mergeCols, 3);

                componentManager.addComponent(component, inComponentDB);
            }
        }
    }
    else
    {
        Widget_air_diff_inputTable* inputTable = new Widget_air_diff_inputTable(true, nullptr);
        inputTable->initTableWidget();
        inputTable->showConfirmButton();
        inputTable->show();
        inputTable->loadComponentToTable();
        connect(inputTable, &Widget_base_inputTable::confirmed, this, &Widget_air_diff_inputTable::handleConfirmation);
    }
}

void Widget_air_diff_inputTable::onDel()
{
    deleteRowFromTable(ui->tableWidget, 3);

    mergeColumnsByNames(ui->tableWidget, mergeCols, 3);
}

void Widget_air_diff_inputTable::onRevise()
{
    for (int row = 0; row < ui->tableWidget->rowCount(); row += 3) {
        QWidget* widget = ui->tableWidget->cellWidget(row, 0); // Assuming the checkbox is in the first column
        QCheckBox* checkBox = widget ? widget->findChild<QCheckBox*>() : nullptr;
        if (checkBox && checkBox->isChecked()) {
            componentRevision<AirDiff, Dialog_air_diff>(ui->tableWidget, 3, row);
        }
    }
    mergeColumnsByNames(ui->tableWidget, mergeCols, 3);
}

void Widget_air_diff_inputTable::onInput()
{

}

void Widget_air_diff_inputTable::onOutput()
{

}

void Widget_air_diff_inputTable::filterByBrandModel(int air_distributor_brandIndex, int air_distributor_modelIndex,
                                                    int diffuser_brandIndex, int diffuser_modelIndex,
                                                    QString air_distributor_brand, QString air_distributor_model,
                                                    QString diffuser_brand, QString diffuser_model)
{
    if(air_distributor_brand == "所有布风器品牌" && diffuser_brand == "所有散流器品牌"
        && (air_distributor_model.isEmpty() || air_distributor_model == "所有布风器型号")
        && (diffuser_model.isEmpty() || diffuser_model == "所有散流器型号"))
    {
        for (int i = 0; i < ui->tableWidget->rowCount(); ++i) {
            ui->tableWidget->setRowHidden(i, false);
        }
    }
    else if(air_distributor_brand == "所有布风器品牌" && diffuser_brand != "所有散流器品牌")
    {
        // 遍历表格中的行，考虑mergeRowCount定义的行合并
        for (int i = 0; i < ui->tableWidget->rowCount(); i += mergeRowCount) {
            bool match = true;

            // 检查当前行和下一行是否匹配品牌
            for (int j = 0; j < mergeRowCount; ++j) {
                int currentRow = i + j;
                if (currentRow >= ui->tableWidget->rowCount()) break;

                QTableWidgetItem *diffuser_brandItem = ui->tableWidget->item(currentRow, diffuser_brandIndex);
                QTableWidgetItem *diffuser_modelItem = ui->tableWidget->item(currentRow, diffuser_modelIndex);

                // 检查品牌匹配，如果品牌匹配且未选择所有型号，则检查型号
                if (!diffuser_brandItem || diffuser_brandItem->text() != diffuser_brand
                    || (diffuser_model != "所有散流器型号" && (!diffuser_modelItem || diffuser_modelItem->text() != diffuser_model))) {
                    match = false;
                    break;
                }
            }

            // 根据匹配结果显示或隐藏整个逻辑行
            for (int j = 0; j < mergeRowCount; ++j) {
                int currentRow = i + j;
                if (currentRow >= ui->tableWidget->rowCount()) break;

                ui->tableWidget->setRowHidden(currentRow, !match);
            }
        }
    }
    else if(air_distributor_brand != "所有布风器品牌" && diffuser_brand == "所有散流器品牌")
    {
        // 遍历表格中的行，考虑mergeRowCount定义的行合并
        for (int i = 0; i < ui->tableWidget->rowCount(); i += mergeRowCount) {
            bool match = true;

            // 检查当前行和下一行是否匹配品牌
            for (int j = 0; j < mergeRowCount; ++j) {
                int currentRow = i + j;
                if (currentRow >= ui->tableWidget->rowCount()) break;

                QTableWidgetItem *air_distributor_brandItem = ui->tableWidget->item(currentRow, air_distributor_brandIndex);
                QTableWidgetItem *air_distributor_modelItem = ui->tableWidget->item(currentRow, air_distributor_modelIndex);

                // 检查品牌匹配，如果品牌匹配且未选择所有型号，则检查型号
                if (!air_distributor_brandItem || air_distributor_brandItem->text() != air_distributor_brand
                    || (air_distributor_model != "所有布风器型号" && (!air_distributor_modelItem || air_distributor_modelItem->text() != air_distributor_model))) {
                    match = false;
                    break;
                }
            }

            // 根据匹配结果显示或隐藏整个逻辑行
            for (int j = 0; j < mergeRowCount; ++j) {
                int currentRow = i + j;
                if (currentRow >= ui->tableWidget->rowCount()) break;

                ui->tableWidget->setRowHidden(currentRow, !match);
            }
        }
    }
    else
    {
        // 遍历表格中的行，考虑mergeRowCount定义的行合并
        for (int i = 0; i < ui->tableWidget->rowCount(); i += mergeRowCount) {
            bool match = true;

            // 检查当前行和下一行是否匹配品牌
            for (int j = 0; j < mergeRowCount; ++j) {
                int currentRow = i + j;
                if (currentRow >= ui->tableWidget->rowCount()) break;

                QTableWidgetItem *diffuser_brandItem = ui->tableWidget->item(currentRow, diffuser_brandIndex);
                QTableWidgetItem *diffuser_modelItem = ui->tableWidget->item(currentRow, diffuser_modelIndex);
                QTableWidgetItem *air_distributor_brandItem = ui->tableWidget->item(currentRow, air_distributor_brandIndex);
                QTableWidgetItem *air_distributor_modelItem = ui->tableWidget->item(currentRow, air_distributor_modelIndex);

                // 检查品牌匹配，如果品牌匹配且未选择所有型号，则检查型号
                if ((!air_distributor_brandItem || air_distributor_brandItem->text() != air_distributor_brand
                     || (air_distributor_model != "所有布风器型号" && (!air_distributor_modelItem || air_distributor_modelItem->text() != air_distributor_model)))
                    &&
                    (!diffuser_brandItem || diffuser_brandItem->text() != diffuser_brand
                     || (diffuser_model != "所有散流器型号" && (!diffuser_modelItem || diffuser_modelItem->text() != diffuser_model)))) {
                    match = false;
                    break;
                }
            }

            // 根据匹配结果显示或隐藏整个逻辑行
            for (int j = 0; j < mergeRowCount; ++j) {
                int currentRow = i + j;
                if (currentRow >= ui->tableWidget->rowCount()) break;

                ui->tableWidget->setRowHidden(currentRow, !match);
            }
        }
    }
}

void Widget_air_diff_inputTable::onFilter()
{
    QDialog* dialog = new QDialog(nullptr);
    dialog->setWindowTitle("筛选");
    QString dialogStyleSheet =
        "QComboBox { height: 25px; font:14px \"黑体\"; }"
        "QComboBox QAbstractItemView::item { height: 20px; font:13px \"黑体\";} "
        "QLabel { height: 25px; font:14px \"黑体\";}"
        "QRadioButton {font:14px \"黑体\";}"
        "QGroupBox {font:13px \"黑体\";}"
        "QPushButton {width:40px; height: 22px; font:14px \"黑体\";}";
    dialog->setStyleSheet(dialogStyleSheet);

    QVBoxLayout* dialogLayout = new QVBoxLayout(dialog);    // 总布局

    QGroupBox* radioBtnGroup = new QGroupBox(nullptr);
    QRadioButton* onlyAirDistributor = new QRadioButton("仅布风器");
    QRadioButton* onlyDiffuser = new QRadioButton("仅散流器");
    QRadioButton* both = new QRadioButton("布风器+散流器");
    QHBoxLayout* radioBtnLayout = new QHBoxLayout(nullptr);      // 单选按钮布局
    radioBtnGroup->setLayout(radioBtnLayout);
    radioBtnGroup->setTitle("筛选条件");

    radioBtnLayout->addWidget(onlyAirDistributor);
    radioBtnLayout->addWidget(onlyDiffuser);
    radioBtnLayout->addWidget(both);
    radioBtnLayout->setSpacing(12);
    radioBtnLayout->setSizeConstraint(QLayout::SetMaximumSize);

    QLabel* label_air_brand = new QLabel("布风器品牌");
    QLabel* label_air_model = new QLabel("布风器型号");
    QComboBox* cBox_air_brand = new QComboBox;
    QComboBox* cBox_air_model = new QComboBox;

    QLabel* label_diff_brand = new QLabel("散流器品牌");
    QLabel* label_diff_model = new QLabel("散流器型号");
    QComboBox* cBox_diff_brand = new QComboBox;
    QComboBox* cBox_diff_model = new QComboBox;

    // 设置 QLabel 的对齐方式
    label_air_brand->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);
    label_air_model->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);
    label_diff_brand->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);
    label_diff_model->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);
    label_air_brand->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    label_air_model->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    label_diff_brand->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    label_diff_model->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);

    // 设置 QComboBox 的大小策略为 Expanding
    cBox_air_brand->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    cBox_air_model->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    cBox_diff_brand->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    cBox_diff_model->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    cBox_air_brand->setView(new QListView());
    cBox_air_model->setView(new QListView());
    cBox_diff_brand->setView(new QListView());
    cBox_diff_model->setView(new QListView());

    QGridLayout* comComboBoxLayout = new QGridLayout(nullptr);   // 格栅布局

    comComboBoxLayout->addWidget(label_air_brand, 0, 0);
    comComboBoxLayout->addWidget(cBox_air_brand, 0, 1);
    comComboBoxLayout->addWidget(label_air_model, 1, 0);
    comComboBoxLayout->addWidget(cBox_air_model, 1, 1);
    comComboBoxLayout->addWidget(label_diff_brand, 2, 0);
    comComboBoxLayout->addWidget(cBox_diff_brand, 2, 1);
    comComboBoxLayout->addWidget(label_diff_model, 3, 0);
    comComboBoxLayout->addWidget(cBox_diff_model, 3, 1);

    QHBoxLayout* btnLayout = new QHBoxLayout(nullptr);
    QPushButton* okBtn = new QPushButton("确认");
    okBtn->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    btnLayout->addStretch();
    btnLayout->addWidget(okBtn);

    dialogLayout->addWidget(radioBtnGroup);
    dialogLayout->addLayout(comComboBoxLayout);
    dialogLayout->addLayout(btnLayout);

    auto updateLayout = [=](bool showAir, bool showDiff) {
        if(label_air_brand->isVisible())
        {
            label_air_brand->setVisible(showAir);
            cBox_air_brand->setVisible(showAir);
            label_air_model->setVisible(showAir);
            cBox_air_model->setVisible(showAir);
            label_diff_brand->setVisible(showDiff);
            cBox_diff_brand->setVisible(showDiff);
            label_diff_model->setVisible(showDiff);
            cBox_diff_model->setVisible(showDiff);
        }
        else
        {
            label_diff_brand->setVisible(showDiff);
            cBox_diff_brand->setVisible(showDiff);
            label_diff_model->setVisible(showDiff);
            cBox_diff_model->setVisible(showDiff);
            label_air_brand->setVisible(showAir);
            cBox_air_brand->setVisible(showAir);
            label_air_model->setVisible(showAir);
            cBox_air_model->setVisible(showAir);
        }
        comComboBoxLayout->invalidate();
        dialog->adjustSize();
    };

    auto addComboBoxItem = [=](QString colName, QComboBox* comboBox) {
        comboBox->blockSignals(true);
        comboBox->clear();
        comboBox->addItem(QString("所有%1").arg(colName));
        int index = -1;
        for (int i = 0; i < ui->tableWidget->columnCount(); ++i) {
            if (ui->tableWidget->horizontalHeaderItem(i)->text() == colName) {
                index = i;
                break;
            }
        }
        for(int i = 0; i < ui->tableWidget->rowCount(); i++)
        {
            QString brand = ui->tableWidget->item(i, index)->text();
            if(comboBox->findText(brand) == -1)
                comboBox->addItem(brand);
        }
        comboBox->setCurrentIndex(0);
        comboBox->blockSignals(false);
    };

    onlyAirDistributor->setChecked(true);
    updateLayout(true, false);
    addComboBoxItem("布风器品牌", cBox_air_brand);
    addComboBoxItem("布风器型号", cBox_air_model);

    connect(onlyAirDistributor, &QRadioButton::clicked, this, [=]() {
        updateLayout(true, false);
        addComboBoxItem("布风器品牌", cBox_air_brand);
        addComboBoxItem("布风器型号", cBox_air_model);
    });

    connect(onlyDiffuser, &QRadioButton::clicked, this, [=]() {
        updateLayout(false, true);
        addComboBoxItem("散流器品牌", cBox_diff_brand);
        addComboBoxItem("散流器型号", cBox_diff_model);
    });

    connect(both, &QRadioButton::clicked, this, [=]() {
        updateLayout(true, true);
        addComboBoxItem("布风器品牌", cBox_air_brand);
        addComboBoxItem("布风器型号", cBox_air_model);
        addComboBoxItem("散流器品牌", cBox_diff_brand);
        addComboBoxItem("散流器型号", cBox_diff_model);
    });

    dialog->setLayout(dialogLayout);
    dialog->exec();

    dialog->deleteLater();
}

void Widget_air_diff_inputTable::onGenerateTemplate()
{

}

void Widget_air_diff_inputTable::loadComponentToTable()
{
    // 获取所有AirDiff组件
    auto componentList = ComponentManager::getInstance().getComponentsByType(inComponentDB, component_type_name::AIRDIFF);
    for (const auto& component : componentList) {
        // 使用dynamic_cast尝试将ComponentBase转换为AirDiff指针
        if (auto airDiffComponent = dynamic_cast<AirDiff*>(component.data())) {
            // 获取组件数据
            auto lists = airDiffComponent->getComponentDataAsStringList(inComponentDB);

            // 假定lists中的数据已经正确分组对应噪声、衰减、反射
            if (lists.size() >= 3) {
                // 添加数据到对应的表格
                addRowToTable(ui->tableWidget, lists[0]);
                addRowToTable(ui->tableWidget, lists[1]);
                addRowToTable(ui->tableWidget, lists[2]);
            }
        }
    }
    mergeColumnsByNames(ui->tableWidget, mergeCols, 3);
}

void Widget_air_diff_inputTable::handleConfirmation(QSet<QString> uuids)
{
    for(auto& uuid : uuids)
    {
        QSharedPointer<ComponentBase> componentBase = ComponentManager::getInstance().findComponent(true, uuid);;
        if(QSharedPointer<AirDiff> component = qSharedPointerCast<AirDiff>(componentBase))
        {
            // 使用深拷贝构造函数来创建一个新的 Fan 对象
            QSharedPointer<AirDiff> newComponent = QSharedPointer<AirDiff>(new AirDiff(*component));
            if (newComponent != nullptr) {

                auto lists = newComponent->getComponentDataAsStringList(false);

                // 使用通用函数添加行
                addRowToTable(ui->tableWidget, lists[0]);
                addRowToTable(ui->tableWidget, lists[1]);
                addRowToTable(ui->tableWidget, lists[2]);

                componentManager.addComponent(newComponent, false);

                // 重新编号
                for (int row = 0; row < ui->tableWidget->rowCount(); ++row) {
                    QTableWidgetItem* item = new QTableWidgetItem(QString::number(row / 3 + 1));
                    ui->tableWidget->setItem(row, 1, item); // Assuming the sequence numbers are in the second column (index 1)
                    item->setTextAlignment(Qt::AlignCenter);
                    item->setFlags(Qt::ItemIsEditable);
                    item->setBackground(QBrush(Qt::lightGray));
                    item->setData(Qt::ForegroundRole, QColor(70, 70, 70));
                }

                // 更新组件信息
                for (int row = 0; row < ui->tableWidget->rowCount(); row += 3) {
                    QString uuid = ui->tableWidget->item(row, ui->tableWidget->columnCount() - 1)->text(); // 获取组件uuid
                    QSharedPointer<ComponentBase> component = componentManager.findComponent(inComponentDB, uuid); // 查找组件

                    if (component) {
                        component->setTableID((QString::number((row + 3) / 3))); // 设置新的table_id，假设组件有这个方法
                        componentManager.updateComponent(uuid, component, inComponentDB); // 更新组件
                    }
                }

                mergeColumnsByNames(ui->tableWidget, mergeCols, 3);
            }
        }
    }
}
