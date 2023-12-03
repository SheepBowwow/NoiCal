#include "inputDialog/dialog_silencer.h"
#include "ui_dialog_silencer.h"
#include "databasemanager.h"
#include "global_constant.h"
#include "globle_var.h"
#include <QDir>
#include <cmath>
#include <QSet>

Dialog_silencer::Dialog_silencer(QString type, QWidget *parent, int editRow,  const Silencer_atten& data) :
    InputBaseDialog(parent),
    editRow(editRow), // 初始化editRow
    ui(new Ui::Dialog_silencer)
{
    ui->setupUi(this);
    setTopWidget(ui->widget_top);  // 设置顶部部件

    if(type == "圆形消音器")
    {
        ui->stackedWidget_title->setCurrentWidget(ui->page_cs);
    }
    else if(type == "矩形消音器")
    {
        ui->stackedWidget_title->setCurrentWidget(ui->page_rs);
    }
    else if(type == "圆形消音弯头")
    {
        ui->stackedWidget_title->setCurrentWidget(ui->page_cse);
    }
    else if(type == "矩形消音弯头")
    {
        ui->stackedWidget_title->setCurrentWidget(ui->page_rse);
    }

    DBManager->executeSqlQuery(roundSilencerModel,"SELECT " + QString(SILENCER_MODEL) + " FROM " + SILENCER_TABLE + " WHERE silencer_type = '圆形消声器'");
    DBManager->executeSqlQuery(rectSilencerModel,"SELECT " + QString(SILENCER_MODEL) + " FROM " + SILENCER_TABLE + " WHERE silencer_type = '方形消音器'");
    DBManager->executeSqlQuery(pipeSilencerModel,"SELECT " + QString(SILENCER_MODEL) + " FROM " + SILENCER_TABLE + " WHERE silencer_type = '消音管'");

    setPrefix();

    if(editRow != -1)
    {
        /***使用-分割前后缀***/
        QString model_prefix = "";
        QString model_suffix = "";
        int slashIndex = data.model.indexOf('-');
        if (slashIndex != -1 && slashIndex < data.model.size() - 1)
        {
            model_prefix = data.model.left(slashIndex).trimmed(); // 获取斜杠之前的部分作为前缀
            model_suffix = "-" + data.model.mid(slashIndex + 1).trimmed(); // 获取斜杠之后的部分作为后缀
        }
        else
        {
            // 如果没有斜杠，整个字符串作为前缀，后缀为空字符串
            model_prefix = data.model.trimmed();
            model_suffix = QString();
        }
        /***使用-分割前后缀***/

        ui->lineEdit_brand->setText(data.brand);
        ui->comboBox_prefix->setCurrentText(model_prefix);
        ui->comboBox_suffix->setCurrentText(model_suffix);
        ui->lineEdit_63->setText(data.noi_63);
        ui->lineEdit_125->setText(data.noi_125);
        ui->lineEdit_250->setText(data.noi_250);
        ui->lineEdit_500->setText(data.noi_500);
        ui->lineEdit_1k->setText(data.noi_1k);
        ui->lineEdit_2k->setText(data.noi_2k);
        ui->lineEdit_4k->setText(data.noi_4k);
        ui->lineEdit_8k->setText(data.noi_8k);
    }


    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint | Qt::WindowStaysOnTopHint);
}

Dialog_silencer::~Dialog_silencer()
{
    delete ui;
}

void* Dialog_silencer::getNoi()
{
    return noi == nullptr ? nullptr : noi;
}


//点击确认键
void Dialog_silencer::on_pushButton_confirm_clicked()
{
    this->noi = new Silencer_atten;

    // 获取对应行的数据，将界面上的数据保存到对应行中
    noi->brand = ui->lineEdit_brand->text();
    noi->model_prefix = ui->comboBox_prefix->currentText();
    noi->model_suffix = ui->comboBox_suffix->currentText();
    noi->noi_63 = ui->lineEdit_63->text();
    noi->noi_125 = ui->lineEdit_125->text();
    noi->noi_250 = ui->lineEdit_250->text();
    noi->noi_500 = ui->lineEdit_500->text();
    noi->noi_1k = ui->lineEdit_1k->text();
    noi->noi_2k = ui->lineEdit_2k->text();
    noi->noi_4k = ui->lineEdit_4k->text();
    noi->noi_8k = ui->lineEdit_8k->text();
    noi->model = noi->model_prefix + noi->model_suffix;
    noi->number = noi->model;

    if(ui->stackedWidget_title->currentWidget() == ui->page_cs)
    {
        noi->type = "圆形消音器";
    }
    else if(ui->stackedWidget_title->currentWidget() == ui->page_rs)
    {
        noi->type = "矩形消音器";
    }
    else if(ui->stackedWidget_title->currentWidget() == ui->page_cse)
    {
        noi->type = "圆形消音弯头";
    }
    else if(ui->stackedWidget_title->currentWidget() == ui->page_rse)
    {
        noi->type = "矩形消音弯头";
    }

    this->accept(); // 关闭对话框
}

void Dialog_silencer::on_close_clicked()
{
    this->close();
}

void Dialog_silencer::setPrefix()
{
    ui->comboBox_prefix->clear();
    QSet<QString> addedPrefixes;
    if(ui->stackedWidget_title->currentWidget() == ui->page_cs)
    {
        /******设置combobox******/
        for(int i = 0; i < roundSilencerModel.size(); i++)
        {
            QString s = roundSilencerModel[i];
            QString letters;

            for (int j = 0; j < s.size(); j++)
            {
                QChar ch = s[j];
                if (!ch.isLetter())
                {
                    break;
                }
                else
                {
                    letters.append(ch);
                }
            }
            // 检查是否已经添加过该前缀
            if (!addedPrefixes.contains(letters))
            {
                ui->comboBox_prefix->addItem(letters);
                addedPrefixes.insert(letters); // 将前缀加入到集合中，以便后续检查重复
            }
        }
        /******设置combobox******/
    }
    else if(ui->stackedWidget_title->currentWidget() == ui->page_rs)
    {
        /******设置combobox******/
        for(int i = 0; i < rectSilencerModel.size(); i++)
        {
            QString s = rectSilencerModel[i];
            QString letters;

            for (int j = 0; j < s.size(); j++)
            {
                QChar ch = s[j];
                if (!ch.isLetter())
                {
                    break;
                }
                else
                {
                    letters.append(ch);
                }
            }
            // 检查是否已经添加过该前缀
            if (!addedPrefixes.contains(letters))
            {
                ui->comboBox_prefix->addItem(letters);
                addedPrefixes.insert(letters); // 将前缀加入到集合中，以便后续检查重复
            }
        }
        /******设置combobox******/
    }
    else if(ui->stackedWidget_title->currentWidget() == ui->page_cse)
    {
        /******设置combobox******/
        for(int i = 0; i < pipeSilencerModel.size(); i++)
        {
            QString s = pipeSilencerModel[i];
            QStringList parts = s.split('-'); // 使用split函数按照斜杠分割字符串

            // parts中的第一个元素是前缀
            QString prefix = parts.first().trimmed(); // 去除前缀两边的空格

            // 检查是否已经添加过该前缀
            if (!addedPrefixes.contains(prefix))
            {
                ui->comboBox_prefix->addItem(prefix);
                addedPrefixes.insert(prefix); // 将前缀加入到集合中，以便后续检查重复
            }
        }
        /******设置combobox******/
    }
    else if(ui->stackedWidget_title->currentWidget() == ui->page_rse)
    {
        /******设置combobox******/
        for(int i = 0; i < pipeSilencerModel.size(); i++)
        {
            QString s = pipeSilencerModel[i];
            QStringList parts = s.split('-'); // 使用split函数按照斜杠分割字符串

            // parts中的第一个元素是前缀
            QString prefix = parts.first().trimmed(); // 去除前缀两边的空格

            // 检查是否已经添加过该前缀
            if (!addedPrefixes.contains(prefix))
            {
                ui->comboBox_prefix->addItem(prefix);
                addedPrefixes.insert(prefix); // 将前缀加入到集合中，以便后续检查重复
            }
        }
        /******设置combobox******/
    }
}

void Dialog_silencer::on_comboBox_prefix_currentTextChanged(const QString &arg1)
{
    // 清空另一个QComboBox的内容
    ui->comboBox_suffix->clear();

    if(ui->stackedWidget_title->currentWidget() == ui->page_cs)
    {
        // 清空另一个QComboBox的内容
        ui->comboBox_suffix->clear();

        // 遍历pumpSendModel，查找匹配的前缀和后缀
        for (const QString &fullName : roundSilencerModel)
        {
            QString prefix;
            QString suffix;

            // 从fullName中分离前缀和后缀
            bool isPrefix = true;
            for (const QChar &ch : fullName)
            {
                if (ch.isLetter() && isPrefix) // 如果是英文字母且当前在前缀中，添加到前缀中
                {
                    prefix.append(ch);
                }
                else // 如果不是英文字母，认为后面的字符都是后缀
                {
                    suffix.append(ch);
                    isPrefix = false;
                }
            }

            // 如果当前的前缀匹配，将后缀添加到comboBox_suffix
            if (prefix == arg1)
            {
                ui->comboBox_suffix->addItem(suffix);
            }
        }
    }
    else if(ui->stackedWidget_title->currentWidget() == ui->page_rs)
    {
        // 清空另一个QComboBox的内容
        ui->comboBox_suffix->clear();

        // 遍历pumpSendModel，查找匹配的前缀和后缀
        for (const QString &fullName : rectSilencerModel)
        {
            QString prefix;
            QString suffix;

            // 从fullName中分离前缀和后缀
            bool isPrefix = true;
            for (const QChar &ch : fullName)
            {
                if (ch.isLetter() && isPrefix) // 如果是英文字母且当前在前缀中，添加到前缀中
                {
                    prefix.append(ch);
                }
                else // 如果不是英文字母，认为后面的字符都是后缀
                {
                    suffix.append(ch);
                    isPrefix = false;
                }
            }

            // 如果当前的前缀匹配，将后缀添加到comboBox_suffix
            if (prefix == arg1)
            {
                ui->comboBox_suffix->addItem(suffix);
            }
        }
    }
    else if(ui->stackedWidget_title->currentWidget() == ui->page_cse)
    {
        // 清空另一个QComboBox的内容
        ui->comboBox_suffix->clear();

        // 遍历pumpSendModel，查找匹配的前缀和后缀
        for (const QString &fullName : pipeSilencerModel)
        {
            QStringList parts = fullName.split('-'); // 使用split函数按照斜杠分割字符串

            // parts中的第一个元素是前缀，第二个元素是后缀（如果有的话）
            QString prefix = parts.value(0).trimmed(); // 去除前缀两边的空格
            QString suffix = "-" + parts.value(1).trimmed(); // 去除后缀两边的空格

            // 如果当前的前缀匹配，将后缀添加到comboBox_suffix
            if (prefix == arg1)
            {
                ui->comboBox_suffix->addItem(suffix);
            }
        }
    }
    else if(ui->stackedWidget_title->currentWidget() == ui->page_rse)
    {
        // 清空另一个QComboBox的内容
        ui->comboBox_suffix->clear();

        // 遍历pumpSendModel，查找匹配的前缀和后缀
        for (const QString &fullName : pipeSilencerModel)
        {
            QStringList parts = fullName.split('-'); // 使用split函数按照斜杠分割字符串

            // parts中的第一个元素是前缀，第二个元素是后缀（如果有的话）
            QString prefix = parts.value(0).trimmed(); // 去除前缀两边的空格
            QString suffix = "-" + parts.value(1).trimmed(); // 去除后缀两边的空格

            // 如果当前的前缀匹配，将后缀添加到comboBox_suffix
            if (prefix == arg1)
            {
                ui->comboBox_suffix->addItem(suffix);
            }
        }
    }
}


void Dialog_silencer::on_comboBox_suffix_currentTextChanged(const QString &arg1)
{
    int band[8] = {63 , 125, 250, 500, 1000, 2000, 4000, 8000};
    QVector<QLineEdit*> v_lineEdit = {ui->lineEdit_63, ui->lineEdit_125, ui->lineEdit_250, ui->lineEdit_500, ui->lineEdit_1k, ui->lineEdit_2k, ui->lineEdit_4k, ui->lineEdit_8k};
    QString modelName = QString(ui->comboBox_prefix->currentText() + ui->comboBox_suffix->currentText());
    DBManager->queryEightNoi(eightNoi, SILENCER_TABLE, SILENCER_MODEL, modelName);
    for(int i = 0; i < 8; i++)
    {
        v_lineEdit[i]->setText(eightNoi[band[i]]);
    }
}

