#include "dialog_prj_manager.h"
#include "ui_dialog_prj_manager.h"
#include "globle_var.h"
#include "project/projectmanager.h"
#include <QMessageBox>
#include <QStandardItemModel>
#include <QCompleter>

Dialog_prj_manager::Dialog_prj_manager(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_prj_manager)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint | Qt::WindowStaysOnTopHint);

    initComboBox();
}

Dialog_prj_manager::~Dialog_prj_manager()
{
    delete ui;
}

void Dialog_prj_manager::initComboBox()
{
    // 获取项目ID列表
    QSet<QString> projectIDsSet = ProjectManager::getInstance().getProjectIDs();

    // 将QSet转换为QStringList
    QStringList projectIDs(projectIDsSet.begin(), projectIDsSet.end());

    // 使用项目ID列表填充Qui->comboBox_prj_num
    for (const auto& prjNum : projectIDs) {
        ui->comboBox_prj_num->addItem(prjNum);
    }

    // 使ui->comboBox_prj_num可编辑以支持自动完成
    ui->comboBox_prj_num->setEditable(true);

    // 创建一个QCompleter，使用项目ID列表
    QCompleter *completer = new QCompleter(projectIDs, ui->comboBox_prj_num);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setCompletionMode(QCompleter::PopupCompletion);
    completer->setMaxVisibleItems(10);

    // 将QCompleter设置给ui->comboBox_prj_num
    ui->comboBox_prj_num->setCompleter(completer);

    // 设置初始选项为-1，即无选项被选中
    if(!ProjectManager::getInstance().getPrjID().trimmed().isEmpty())
        ui->comboBox_prj_num->setCurrentText(ProjectManager::getInstance().getPrjID());
    else
        ui->comboBox_prj_num->setCurrentIndex(-1);
}

void Dialog_prj_manager::on_pushButton_create_prj_clicked()
{
    if(ui->lineEdit_prj_ID->text().trimmed().isEmpty())
    {
        QMessageBox::critical(this,"错误","项目编号不能为空");
        return;
    }

    //清空界面数据和表格数据
    if(!ProjectManager::getInstance().getPrjID().isEmpty())
    {
        ProjectManager::getInstance().clearCurrentPrjData();
        ComponentManager::getInstance().clearCurrentPrjComponents();
    }

    QString projectID = ui->lineEdit_prj_ID->text();

    if(ProjectManager::getInstance().insertProjectIDToSet(projectID))
    {
        ProjectManager::getInstance().setPrjID(projectID);
        // 关闭新建项目窗口
        accept();
        this->close();
    }
    else
    {
        QMessageBox::critical(this, "新建项目错误", "项目编号重复, 该项目已经存在!");
    }
}

//可以在构造函数中初始一下last变量用其成员函数setX,setY就是了
//接下来就是对三个鼠标事件的重写
void Dialog_prj_manager::mousePressEvent(QMouseEvent *e)
{
    if(e->button()==Qt::LeftButton&&!this->isMaximized())
    {
            //获取相当于窗口的位置                标题栏的高度
        if(mapFromGlobal(QCursor().pos()).y()<ui->widget_top->height())
        {
            last = e->globalPos();
            canmove=1;
        }
    }
}
void Dialog_prj_manager::mouseMoveEvent(QMouseEvent *e)
{
    if(canmove==1)
    {
        int dx = e->globalX() - last.x();
        int dy = e->globalY() - last.y();
        last = e->globalPos();
        move(x()+dx, y()+dy);
    }
}
void Dialog_prj_manager::mouseReleaseEvent(QMouseEvent *e)
{
    if(canmove==1)
    {
        int dx = e->globalX() - last.x();
        int dy = e->globalY() - last.y();
        move(x()+dx, y()+dy);
        canmove=0;
    }
}

void Dialog_prj_manager::on_close_clicked()
{
    this->close();
}

void Dialog_prj_manager::on_pushButton_switch_clicked()
{
    if(ui->comboBox_prj_num->currentText().trimmed().isEmpty() ||
            !DatabaseManager::getInstance().isProjectExist(ui->comboBox_prj_num->currentText()))
    {
        QMessageBox::critical(this, "错误", "不存在该项目或项目名为空");
        return;
    }

    if(ui->comboBox_prj_num->currentText() != ProjectManager::getInstance().getPrjID())
    {
        //清空界面数据和表格数据
        ProjectManager::getInstance().clearCurrentPrjData();            //清空数据, 包括项目基本信息以及所有表格的数据
        ComponentManager::getInstance().clearCurrentPrjComponents();    //清空容器
        //设置新的数据
        ProjectManager::getInstance().setPrjID(ui->comboBox_prj_num->currentText(), false); //设置项目编号, 第二个为是否为新建的项目编号
        ProjectManager::getInstance().switchProjectToDo();  //设置基本信息
        ComponentManager::getInstance().loadComponentToHash();  //添加数据库的数据到容器中
    }

    accept();
    this->close();
}


void Dialog_prj_manager::on_pushButton_del_clicked()
{
    if(ui->comboBox_prj_num->currentText().trimmed().isEmpty() ||
            !DatabaseManager::getInstance().isProjectExist(ui->comboBox_prj_num->currentText()))
    {
        QMessageBox::critical(this, "错误", "不存在该项目或项目名为空");
        return;
    }

    QMessageBox msgBox(this);
    msgBox.setWindowTitle("确认删除");
    msgBox.setText("确认删除项目 " + ui->comboBox_prj_num->currentText() + " 吗?");
    msgBox.setIcon(QMessageBox::Warning);
    QPushButton *yesButton = msgBox.addButton("确认", QMessageBox::YesRole);
    QPushButton *noButton = msgBox.addButton("取消", QMessageBox::NoRole);
    msgBox.exec();

    if (msgBox.clickedButton() == yesButton)
    {
        // 获取当前选中的项目名称
        QString currentProjectName = ui->comboBox_prj_num->currentText();
        // 从数据库中删除项目
        ProjectManager::getInstance().delPrj(currentProjectName);
        int index = ui->comboBox_prj_num->findText(currentProjectName);
        if (index != -1) { // 确认找到了对应的项
            // 删除这个索引对应的项
            ui->comboBox_prj_num->removeItem(index);
        }
        ui->comboBox_prj_num->setCurrentIndex(-1);
        //清空界面数据和表格数据
        ProjectManager::getInstance().clearCurrentPrjData();            //清空数据, 包括项目基本信息以及所有表格的数据
        ComponentManager::getInstance().clearCurrentPrjComponents();    //清空容器
    }
    else if (msgBox.clickedButton() == noButton)
    {
        return;
    }
}

