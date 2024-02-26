#ifndef CLICKABLECOMBOBOX_H
#define CLICKABLECOMBOBOX_H

#include <QObject>
#include <QComboBox>

class ClickableComboBox : public QComboBox
{
    Q_OBJECT

public:
    explicit ClickableComboBox(QWidget *parent = nullptr);

signals:
    void clicked();

protected:
    void mousePressEvent(QMouseEvent *event) override;
};

#endif // CLICKABLECOMBOBOX_H
