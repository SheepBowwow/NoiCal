#include "HeaderViewButtons.h"
#include <QPainter>
#include <QPushButton>
#include <QMouseEvent>

HeaderViewButtons::HeaderViewButtons(int buttonColumnIndex, Qt::Orientation orientation, QWidget* parent)
    : QHeaderView(orientation, parent), m_buttonColumnIndex(buttonColumnIndex)
{
    setSectionsClickable(true);
}

HeaderViewButtons::~HeaderViewButtons()
{
}

void HeaderViewButtons::paintSection(QPainter* painter, const QRect& rect, int logicalIndex) const
{
    QStyleOptionHeader opt;
    initStyleOption(&opt);
    opt.rect = rect;

    // 绘制普通表头
    QHeaderView::paintSection(painter, rect, logicalIndex);

    // 绘制按钮
    if (logicalIndex == count() - 1) {
        // 在最后一列绘制加号按钮
        QPushButton addButton("+", const_cast<HeaderViewButtons*>(this));
        addButton.setGeometry(rect.right() - 20, rect.y() + 2, 18, 18);
        addButton.setStyleSheet("QPushButton { border: 1px solid black; }");
        connect(&addButton, &QPushButton::clicked, this, &HeaderViewButtons::addButtonClicked);
        addButton.render(painter, QPoint(), QRegion(), QWidget::DrawChildren);

        // 在最后一列绘制减号按钮
        QPushButton delButton("-", const_cast<HeaderViewButtons*>(this));
        delButton.setGeometry(rect.right() - 40, rect.y() + 2, 18, 18);
        delButton.setStyleSheet("QPushButton { border: 1px solid black; }");
        connect(&delButton, &QPushButton::clicked, this, &HeaderViewButtons::delButtonClicked);
        delButton.render(painter, QPoint(), QRegion(), QWidget::DrawChildren);
    }
}

void HeaderViewButtons::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        int logicalIndex = logicalIndexAt(event->pos());
        if (logicalIndex == m_buttonColumnIndex)
        {
            if (event->pos().x() >= width() - 20)
            {
                emit addButtonClicked();
            }
            else if (event->pos().x() >= width() - 40)
            {
                emit delButtonClicked();
            }
        }
    }

    QHeaderView::mousePressEvent(event);
}
