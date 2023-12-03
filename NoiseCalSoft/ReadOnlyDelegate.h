#ifndef READONLYDELEGATE_H
#define READONLYDELEGATE_H

#include <QApplication>
#include <QTableWidget>
#include <QStyledItemDelegate>
#include <QPainter>

class ReadOnlyDelegate : public QStyledItemDelegate {
public:
    explicit ReadOnlyDelegate(QObject* parent = nullptr) : QStyledItemDelegate(parent) {}

    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override {
        if (!(index.flags() & Qt::ItemIsEditable)) {
            // 如果单元格只读，设置背景颜色为灰色
            painter->fillRect(option.rect, Qt::lightGray);
        }

        // 绘制文本
        QStyledItemDelegate::paint(painter, option, index);
    }
};


#endif // READONLYDELEGATE_H
