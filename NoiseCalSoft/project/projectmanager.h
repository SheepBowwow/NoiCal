#ifndef PROJECTMANAGER_H
#define PROJECTMANAGER_H

#include <QObject>
#include <QSet>

//项目基本信息
struct ProjectInfo
{
    QString prjID;      //项目编号
    QString prjName;    //项目名称
    QString shipName;   //船名
    QString shipyard;   //船厂
    QString prjManager; //负责人
    QString classSoc;   //船级社 Classification Society
};

//项目附件
struct ProjectAttachment
{
    QString tableID;        //表格序号
    QString attachName;     //附件名
    QString attachPath;     //附件路径
};

//参考图纸
struct Drawing
{
    QString tableID;        //表格序号
    QString drawingNum;     //图号
    QString drawingName;    //图名
};

//噪声要求
struct NoiseLimit
{
    QString tableID;        //表格序号
    QString roomType;       //房间类型
    QString noiseLimit;     //噪声限值
    QString premissType;    //处所类型
};


class ProjectManager
{
public:
    // 删除或隐藏构造函数
    static ProjectManager& getInstance() {
        static ProjectManager instance; // Guaranteed to be destroyed and instantiated on first use.
        return instance;
    }

    // 确保单例不能被复制
    ProjectManager(ProjectManager const&) = delete; // Delete copy constructor
    void operator=(ProjectManager const&) = delete; // Delete copy assignment operator

    void loadBoatNumSet();

    QSet<QString> getProjectIDs() const;

    void setPrjInfo(const ProjectInfo &newPrjInfo);

    void setPrjID(const QString &prjID);

private:
    ProjectManager(); // 私有构造函数
    ~ProjectManager() {} // 私有析构函数，如果需要的话
    ProjectInfo prjInfo;
    QList<ProjectAttachment> attachments;   //项目附件
    QList<Drawing> drawings;                //参考图纸清单
    QList<NoiseLimit> noiseLimits;          //噪声要求

    QSet<QString> projectIDs;   //用来存储项目编号的
};

#endif // PROJECTMANAGER_H
