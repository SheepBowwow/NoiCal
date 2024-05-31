#ifndef PROJECTMANAGER_H
#define PROJECTMANAGER_H

#include <QObject>
#include <QSet>

//项目基本信息
struct ProjectInfo
{
    QString prjID;      //项目编号
    QString prjName;    //项目名称
    QString shipNum;   //船名
    QString shipyard;   //船厂
    QString prjManager; //负责人
    QString classSoc;   //船级社 Classification Society

    ProjectInfo() = default;

    // 构造函数
    ProjectInfo(const QString& id, const QString& name, const QString& num, const QString& yard, const QString& manager, const QString& soc)
    : prjID(id), prjName(name), shipNum(num), shipyard(yard), prjManager(manager), classSoc(soc)
    {
    }

    // 检查ProjectInfo是否有效
    bool isValid() const {
        return !prjID.isEmpty(); // 这里我们仅以prjID非空为有效性判断依据，你可以根据需要调整
    }
};

//项目附件
struct ProjectAttachment
{
    QString tableID;        //表格序号
    QString attachName;     //附件名
    QString attachPath;     //附件路径

    // 重载 operator==
    bool operator==(const ProjectAttachment& other) const {
        return (tableID == other.tableID) &&
                (attachName == other.attachName) &&
                (attachPath == other.attachPath);
    }
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


class ProjectManager : public QObject
{
    Q_OBJECT
public:
    // 删除或隐藏构造函数
    static ProjectManager& getInstance() {
        static ProjectManager instance; // Guaranteed to be destroyed and instantiated on first use.
        return instance;
    }

    // 确保单例不能被复制
    ProjectManager(ProjectManager const&) = delete; // Delete copy constructor
    void operator=(ProjectManager const&) = delete; // Delete copy assignment operator

    bool isProjectOpened(); //返回当前是否有项目打开

    QSet<QString> getProjectIDs() const;

    void setPrjInfo(const ProjectInfo &newPrjInfo);

    void setPrjID(const QString &prjID, bool initProject = true);

    void delPrj(const QString &prjID);

    QString getPrjID() const;

    void setClassSoc(const QString &classSoc);

    QString getClassSoc() const;

    void clearCurrentPrjData();     //用于切换项目用

    bool insertProjectIDToSet(const QString &prjID);
    bool insertAttachmentToList(const ProjectAttachment &attachment);
    bool removeAttachmentInList(const QString &attachmentName);
    void setDrawings(const QList<Drawing> &drawings);
    bool setNoiseLimits(const QList<NoiseLimit> &noiseLimits);
    void clearAttachments();
    void clearDrawings();
    void clearNoiseLimits();

    bool loadProjectInfo();
    bool loadAttachments();
    bool loadDrawings();
    bool loadNoiseLimits();
    bool switchProjectToDo();

    QList<ProjectAttachment> getAttachments() const;

    QList<Drawing> getDrawings() const;

    QList<NoiseLimit> getNoiseLimits() const;

    ProjectInfo getPrjInfo() const;

signals:
    void clearTable();
    void loadBasicInfoDone();

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
