#ifndef HEADERVIEWBUTTONS_H
#define HEADERVIEWBUTTONS_H

#include <QHeaderView>

class HeaderViewButtons : public QHeaderView
{
    Q_OBJECT
public:
    HeaderViewButtons(int buttonColumnIndex, Qt::Orientation orientation, QWidget* parent = nullptr);
    ~HeaderViewButtons();

signals:
    void addButtonClicked();
    void delButtonClicked();

protected:
    void paintSection(QPainter* painter, const QRect& rect, int logicalIndex) const override;
    void mousePressEvent(QMouseEvent* event) override;

private:
    int m_buttonColumnIndex;
};

#endif // HEADERVIEWBUTTONS_H
