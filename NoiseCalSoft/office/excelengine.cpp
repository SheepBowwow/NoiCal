#include "excelengine.h"
#include <QTableWidget>
#include <QDateTime>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <QApplication>
#include <QTimer>
#include <QPushButton>
#include <QAxObject>

ExcelEngine::ExcelEngine(QWidget *parent) :
    _parent(parent)
{

}

QList<MergedCellInfo> ExcelEngine::getMergedCells(QTableWidget *tableWidget) {
    QList<MergedCellInfo> mergedCells;

    for (int col = 1; col < tableWidget->columnCount(); ++col) {
        for (int row = 1; row < tableWidget->rowCount(); ++row) {
            // 检查当前单元格是否已经被处理
            if (tableWidget->item(row, col) != nullptr) {
                // 获取合并的行数
                int rowSpan = tableWidget->rowSpan(row, col);

                // 如果当前单元格是合并的起始单元格
                if (rowSpan > 1) {
                    mergedCells.append({
                        row,    // 起始行
                        col,    // 列（可以省略，或者保持以备将来使用）
                        rowSpan // 合并的行数
                    });
                }

                // 跳过合并的单元格，避免重复处理
                row += rowSpan - 1; // 跳到合并的最后一行
            }
        }
    }

    return mergedCells;
}

QAxObject *ExcelEngine::createExcelObject() {
    QAxObject* excel = new QAxObject();
    excel->setControl("Excel.Application");
    if (excel->isNull()) {
        throw std::runtime_error("未能创建 Excel 对象，请安装 Microsoft Excel。");
    }
    excel->dynamicCall("SetVisible (bool Visible)", "false");
    excel->setProperty("DisplayAlerts", false);
    return excel;
}

bool ExcelEngine::setTableHeaders(QTableWidget *table, QAxObject *worksheet) {
    for (int i = 1; i < table->columnCount() - 1; i++) {
        if (table->horizontalHeaderItem(i + 1) != nullptr) {
            QAxObject* range = worksheet->querySubObject("Cells(int,int)", 1, i);
            range->dynamicCall("SetValue(const QString &)", table->horizontalHeaderItem(i + 1)->text());
        }
    }
    return true;
}

bool ExcelEngine::setTableData(QTableWidget *table, QAxObject *worksheet) {
    for (int i = 1; i < table->rowCount() + 1; i++) {
        for (int j = 1; j < table->columnCount() - 1; j++) {
            if (table->item(i - 1, j + 1) != nullptr) {
                QAxObject* range = worksheet->querySubObject("Cells(int,int)", i + 1, j);
                range->dynamicCall("SetValue(const QString &)", table->item(i - 1, j + 1)->data(Qt::DisplayRole).toString());
            }
        }
    }
    return true;
}

bool ExcelEngine::deriveExecl(QTableWidget *table, QString component_name) {
    QDateTime currentDateTime = QDateTime::currentDateTime();
    QString defaultFileName = component_name + "_" + currentDateTime.toString("yyyyMMdd_hhmmss") + ".xlsx";
    QString filepath = QFileDialog::getSaveFileName(_parent, tr("Save"), defaultFileName, tr(" (*.xlsx)"));

    if (!filepath.isEmpty()) {
        // 显示正在导出提示框
        QMessageBox* msgBox = new QMessageBox(_parent);
        msgBox->setProperty("customMsgBox", true);
        msgBox->setWindowTitle("提示");
        msgBox->setText("正在导出，请稍候...");
        msgBox->setStandardButtons(QMessageBox::NoButton); // 不显示按钮
        msgBox->setModal(true);
        msgBox->show();

        // 处理事件以确保提示框显示
        QApplication::processEvents();

        try {
            QAxObject* excel = createExcelObject();
            QAxObject* workbooks = excel->querySubObject("WorkBooks");
            workbooks->dynamicCall("Add");
            QAxObject* workbook = excel->querySubObject("ActiveWorkBook");
            QAxObject* worksheets = workbook->querySubObject("Sheets");
            QAxObject* worksheet = worksheets->querySubObject("Item(int)", 1);

            // 设置表头（从下标1开始，跳过uuid列）
            for (int col = 1; col < table->columnCount(); ++col) {
                if (table->horizontalHeaderItem(col)->text() != "UUID") {
                    QString cellData = table->horizontalHeaderItem(col)->text();
                    cellData.replace("\n", ""); // 替换换行符为空字符串

                    QAxObject* range = worksheet->querySubObject("Cells(int,int)", 1, col);
                    range->dynamicCall("SetValue(const QString&)", cellData);
                }
            }

            // 设置数据（从下标1开始，跳过uuid列）
            for (int row = 0; row < table->rowCount(); ++row) {
                for (int col = 1; col < table->columnCount(); ++col) {
                    if (table->horizontalHeaderItem(col)->text() != "UUID" && table->item(row, col) != nullptr) {
                        QString cellData = table->item(row, col)->data(Qt::DisplayRole).toString();
                        cellData.replace("\n", ""); // 替换换行符为空字符串

                        QAxObject* range = worksheet->querySubObject("Cells(int,int)", row + 2, col); // row + 2 以跳过表头
                        range->dynamicCall("SetValue(const QString&)", cellData);
                    }
                }
            }

            // 自适应列宽
            for (int col = 1; col < table->columnCount(); ++col) {
                if (table->horizontalHeaderItem(col)->text() != "UUID") {
                    QAxObject* column = worksheet->querySubObject("Columns(int)", col);
                    column->dynamicCall("AutoFit()"); // 自适应列宽
                }
            }

            workbook->dynamicCall("SaveAs(const QString&)", QDir::toNativeSeparators(filepath));
            workbook->dynamicCall("Close()");
            excel->dynamicCall("Quit()");
            delete excel;

            // 先关闭提示框
            msgBox->close();
            delete msgBox; // 删除指针以防内存泄漏

            // 使用 QTimer 延迟显示导出成功消息框
            QTimer::singleShot(100, this, [this]() {
                // 创建新的 QMessageBox 并设置属性
                QMessageBox* successMsgBox = new QMessageBox(_parent);
                successMsgBox->setProperty("customMsgBox", true);  // 设置自定义属性
                successMsgBox->setWindowTitle("提示");
                successMsgBox->setText("导出成功");
                successMsgBox->setStandardButtons(QMessageBox::Yes);

                // 修改 Yes 按钮的文本为 "是"
                QAbstractButton* yesButton = successMsgBox->button(QMessageBox::Yes);
                if (yesButton) {
                    QPushButton* button = qobject_cast<QPushButton*>(yesButton);
                    if (button) {
                        button->setText("确定");
                    }
                }

                successMsgBox->exec();  // 使用 exec() 以阻塞方式显示消息框
                delete successMsgBox;   // 删除指针，避免内存泄漏
            });
        } catch (const std::exception& e) {
            msgBox->close();
            delete msgBox; // 删除指针以防内存泄漏
            // 创建新的 QMessageBox 并设置属性
            QMessageBox* errorMsgBox = new QMessageBox(_parent);
            errorMsgBox->setProperty("customMsgBox", true);  // 设置自定义属性
            errorMsgBox->setIcon(QMessageBox::Critical);     // 设置为错误图标
            errorMsgBox->setWindowTitle(tr("错误"));
            errorMsgBox->setText(QString::fromStdString(e.what()));

            // 添加 "确定" 按钮
            errorMsgBox->setStandardButtons(QMessageBox::Ok);

            // 修改按钮文本为 "确定"
            QAbstractButton* okButton = errorMsgBox->button(QMessageBox::Ok);
            if (okButton) {
                QPushButton* button = qobject_cast<QPushButton*>(okButton);
                if (button) {
                    button->setText("确定");
                }
            }

            errorMsgBox->exec();  // 显示消息框
            delete errorMsgBox;   // 防止内存泄漏
            return false;
        } catch (...) { // 捕获其他可能的异常
            msgBox->close();
            delete msgBox; // 删除指针以防内存泄漏
            // 创建新的 QMessageBox 并设置属性
            QMessageBox* errorMsgBox = new QMessageBox(_parent);
            errorMsgBox->setProperty("customMsgBox", true);  // 设置自定义属性
            errorMsgBox->setIcon(QMessageBox::Critical);     // 设置为错误图标
            errorMsgBox->setWindowTitle(tr("错误"));
            errorMsgBox->setText(tr("导出失败，发生未知错误。"));

            // 添加 "确定" 按钮
            errorMsgBox->setStandardButtons(QMessageBox::Ok);

            // 修改按钮文本为 "确定"
            QAbstractButton* okButton = errorMsgBox->button(QMessageBox::Ok);
            if (okButton) {
                QPushButton* button = qobject_cast<QPushButton*>(okButton);
                if (button) {
                    button->setText("确定");
                }
            }

            errorMsgBox->exec();  // 显示消息框
            delete errorMsgBox;   // 防止内存泄漏
            return false;
        }
    }

    return true;
}

bool ExcelEngine::importData(QStringList &dataList)
{
    try {
        // 选择 Excel 文件路径
        QString excelFilePath = QFileDialog::getOpenFileName(_parent, tr("选择 Excel 文件"), QString(), tr("Excel 文件 (*.xlsx)"));

        if (excelFilePath.isEmpty()) {
            return false; // 如果未选择文件，则返回 false
        }

        // 创建导入提示框
        QMessageBox* msgBox = new QMessageBox(_parent);
        msgBox->setProperty("customMsgBox", true);
        msgBox->setWindowTitle("提示");
        msgBox->setText("正在导入，请稍候...");
        msgBox->setStandardButtons(QMessageBox::NoButton); // 不显示按钮
        msgBox->setModal(true);
        msgBox->show();

        // 处理事件以确保提示框显示
        QApplication::processEvents();

        convertExcelToCSV(excelFilePath);  // 调用转换函数
        readCsvData(dataList);             // 读取 CSV 数据

        // 先关闭提示框
        msgBox->close();
        delete msgBox; // 删除指针以防内存泄漏

        // 使用 QTimer 延迟显示导入成功消息框
        QTimer::singleShot(100, this, [this]() {
            // 创建新的 QMessageBox 并设置属性
            QMessageBox* successMsgBox = new QMessageBox(_parent);
            successMsgBox->setProperty("customMsgBox", true);  // 设置自定义属性
            successMsgBox->setWindowTitle("提示");
            successMsgBox->setText("导入成功");
            successMsgBox->setStandardButtons(QMessageBox::Yes);

            // 修改 Yes 按钮的文本为 "是"
            QAbstractButton* yesButton = successMsgBox->button(QMessageBox::Yes);
            if (yesButton) {
                QPushButton* button = qobject_cast<QPushButton*>(yesButton);
                if (button) {
                    button->setText("确定");
                }
            }

            successMsgBox->exec();  // 使用 exec() 以阻塞方式显示消息框
            delete successMsgBox;   // 删除指针，避免内存泄漏
        });

        return true; // 成功导入数据后返回 true

    } catch (const std::exception& e) {
        // 错误处理
        QMessageBox* msgBox = new QMessageBox(_parent);
        msgBox->setProperty("customMsgBox", true);
        msgBox->setIcon(QMessageBox::Critical);
        msgBox->setWindowTitle(tr("错误"));
        msgBox->setText(QString::fromStdString(e.what()));
        msgBox->setStandardButtons(QMessageBox::Ok);

        QAbstractButton* okButton = msgBox->button(QMessageBox::Ok);
        if (okButton) {
            QPushButton* button = qobject_cast<QPushButton*>(okButton);
            if (button) {
                button->setText("确定");
            }
        }

        msgBox->exec();
        delete msgBox;

        return false;
    } catch (...) { // 捕获其他可能的异常
        // 错误处理
        QMessageBox* msgBox = new QMessageBox(_parent);
        msgBox->setProperty("customMsgBox", true);
        msgBox->setIcon(QMessageBox::Critical);
        msgBox->setWindowTitle(tr("错误"));
        msgBox->setText(tr("导入失败，发生未知错误。"));
        msgBox->setStandardButtons(QMessageBox::Ok);

        QAbstractButton* okButton = msgBox->button(QMessageBox::Ok);
        if (okButton) {
            QPushButton* button = qobject_cast<QPushButton*>(okButton);
            if (button) {
                button->setText("确定");
            }
        }

        msgBox->exec();
        delete msgBox;

        return false;
    }
}

void ExcelEngine::convertExcelToCSV(const QString &excelFilePath)
{
    QAxObject* excel = createExcelObject();
    QString appDir = QCoreApplication::applicationDirPath();

    // 打开 Excel 文件
    QAxObject* workbooks = excel->querySubObject("Workbooks");
    QAxObject* workbook = workbooks->querySubObject("Open(const QString&)", excelFilePath);

    if (!workbook) {
        qDebug() << "无法打开工作簿：" << excelFilePath;
        delete excel;
        return;
    }

    // 另存为 CSV
    if (!workbook->dynamicCall("SaveAs(const QString&, int)", QDir::toNativeSeparators(appDir + "/tmp.csv"), 6).toBool()) {
        qDebug() << "CSV 文件保存失败！";
    }

    // 关闭工作簿和 Excel
    workbook->dynamicCall("Close()");
    excel->dynamicCall("Quit()");

    delete excel;
}

void ExcelEngine::readCsvData(QStringList &dataList) {
    QString appDir = QCoreApplication::applicationDirPath();
    QFile file(appDir + "/tmp.csv");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "无法打开文件:" << file.errorString();
        return;
    }

    QTextStream in(&file);
    int lineNumber = 0; // 用于判断是否是第一行
    while (!in.atEnd()) {
        QString line = in.readLine(); // 读取一行

        // 跳过第一行
        if (lineNumber++ < 2) {
            continue;
        }


        // 添加整行内容到 dataList
        dataList.append(line.trimmed()); // 去除多余空格并添加到 QStringList
    }

    file.close();

    // 删除读取后的文件
    if (!file.remove()) {
        qDebug() << "删除文件失败:" << file.errorString();
    }
}


