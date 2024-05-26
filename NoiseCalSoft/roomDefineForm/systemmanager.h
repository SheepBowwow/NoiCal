#ifndef SYSTEMMANAGER_H
#define SYSTEMMANAGER_H

#include <QObject>
#include <QWidget>

namespace noise_src_component
{
const QString AIRCONDITION_SINGLE = QStringLiteral("空调器(单风机)");
const QString AIRCONDITION_DOUBLE = QStringLiteral("空调器(双风机)");
const QString FAN = QStringLiteral("风机");
const QString FANCOIL = QStringLiteral("风机盘管");
}

class SystemManager : public QObject
{
    Q_OBJECT
public:
    // 禁用拷贝构造函数和赋值操作符
    SystemManager(const SystemManager&) = delete;
    SystemManager& operator=(const SystemManager&) = delete;

    // 提供一个静态方法来获取类的实例
    static SystemManager& getInstance() {
        static SystemManager instance; // C++11后，局部静态变量的初始化是线程安全的
        return instance;
    }

    void addComponentToSystemMap(const QString& systemName, const QString& componentType, const QString& coponentUUID);
    QList<QString> getNoiseComponentsInSystem(const QString& systemName, const QString& componentType);

private:
    SystemManager();

    /**
     * @brief systemSrcComponentsMap
     * 用来存储系统清单
     * 模板说明
     * 第一层 QString-Hash systemName-systemList
     * 第二层 QString-QVector noise_src_component(上面的常量)-对应部件的容器
     */
    QHash<QString,QHash<QString,QList<QString>>> systemSrcComponentsMap;
    /**
     * @brief sectionSystemsMap
     * 主竖区-系统
     * 主竖区下会有多个系统
     */
    QHash<QString,QList<QString>> sectionSystemsMap;
};

#endif // SYSTEMMANAGER_H
