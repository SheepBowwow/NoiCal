#ifndef INPUTBASEDIALOG_H
#define INPUTBASEDIALOG_H

#include <QDialog>
#include <QMouseEvent>
#include <QObject>

class InputBaseDialog : public QDialog
{
    Q_OBJECT

public:
    explicit InputBaseDialog(QWidget *parent = nullptr, QWidget *topWidget = nullptr);
    virtual ~InputBaseDialog();
    void setTopWidget(QWidget *topWidget);
    virtual void* getNoi() = 0; // 基类的 getNoi 声明为纯虚函数

protected:
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);

private:
    QPoint last;
    int canmove;
    QWidget *widgetTop;
};

#include <QVBoxLayout>
#include <QLabel>

class ImageDialog : public QDialog {

    Q_OBJECT
public:
    static ImageDialog* getInstance(const QString& imagePath, QWidget* parent = nullptr) {
        if (!instance) {
            instance = new ImageDialog(imagePath, parent);
        }
        return instance;
    }

    static void destroyInstance() {
        if (instance) {
            delete instance;
            instance = nullptr;
        }
    }

    void updateImage(const QString& imagePath) {
        QVBoxLayout* layout = qobject_cast<QVBoxLayout*>(this->layout());
        if (!layout) return; // 安全检查

        // 假设imageLabel是layout的第一个widget
        QLabel* imageLabel = qobject_cast<QLabel*>(layout->itemAt(0)->widget());
        if (imageLabel) {
            QPixmap image(imagePath);
            imageLabel->setPixmap(image);
        }
    }

public slots:
    void closeImageDialog();

private:
    ImageDialog(const QString& imagePath, QWidget* parent = nullptr)
        : QDialog(parent) {

        QVBoxLayout* layout = new QVBoxLayout(this);

        QLabel* imageLabel = new QLabel(this);

        // 使用资源路径加载图片
        QPixmap image(imagePath);
        imageLabel->setPixmap(image);
        imageLabel->setScaledContents(true);

        layout->addWidget(imageLabel);

        setLayout(layout);

        // 设置窗口标志，使其置顶
        setWindowFlags(Qt::Dialog | Qt::WindowStaysOnTopHint);
    }

    static ImageDialog* instance;
};

#endif // INPUTBASEDIALOG_H
