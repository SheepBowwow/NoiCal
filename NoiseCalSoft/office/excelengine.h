#ifndef EXCELENGINE_H
#define EXCELENGINE_H

#include <QObject>
#include <QAxObject>
#include <QAxWidget>

class QTableWidget;
class QFile;

typedef struct _MergedCellInfo {
    int startRow;  // 合并单元格的起始行（从 0 开始）
    int column;    // 合并单元格的起始列（从 0 开始）
    int rowSpan;   // 合并的行数
}MergedCellInfo;

class ExcelEngine : public QObject
{
    Q_OBJECT
public:
    //获取表格合并信息
    QList<MergedCellInfo> getMergedCells(QTableWidget *tableWidget);

    //创建QAxObject
    QAxObject* createExcelObject();
    //设置表头
    bool setTableHeaders(QTableWidget* table, QAxObject* worksheet);
    //设置表格数据
    bool setTableData(QTableWidget* table, QAxObject* worksheet);
    //导出表格
    bool deriveExecl(QTableWidget *table, QString component_name);

    //导入数据
    bool importData(QStringList &dataList);

    ExcelEngine(QWidget* parent = nullptr);

private:
    QWidget* _parent;

    void convertExcelToCSV(const QString &excelFilePath);
    void readCsvData(QStringList &dataList);
};

#endif // EXCELENGINE_H
