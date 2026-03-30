#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QString>
#include <QVariant>
#include <QByteArray>
#include <QDir>
#include <QDebug>
#include <QVariantMap>
#include <QList>

/**
 * @class DataBaseManager
 * @brief 数据库管理类，使用单例模式管理 SQLite 数据库
 * @details 提供数据库初始化、建表、增删改查等功能
 */
class DataBaseManager : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief 获取单例实例
     * @return DataBaseManager& 单例引用
     */
    static DataBaseManager& instance() {
        static DataBaseManager inst;
        return inst;
    }

    /**
     * @brief 析构函数
     */
    ~DataBaseManager() override;

    // 禁止拷贝和赋值
    DataBaseManager(const DataBaseManager&) = delete;
    DataBaseManager& operator=(const DataBaseManager&) = delete;

    /**
     * @brief 检查数据库连接状态
     * @return bool 连接是否成功
     */
    bool isConnected() const;

    /**
     * @brief 获取数据库连接
     * @return QSqlDatabase 数据库连接
     */
    QSqlDatabase& getQSqlDatabase();

    // ==================== studentInfo 表操作 ====================
    /**
     * @brief 插入学生信息
     * @param id 学号
     * @param name 姓名
     * @param gender 性别
     * @param birthday 生日
     * @param joinDate 加入日期
     * @param studyGoal 学习目标
     * @param progress 当前进度
     * @param photo 照片（二进制数据）
     * @return bool 操作是否成功
     */
    bool insertStudentInfo(const QString& id, const QString& name, const QString& gender,
                          const QString& birthday, const QString& joinDate, const QString& studyGoal,
                          const QString& progress, const QByteArray& photo = QByteArray());

    /**
     * @brief 查询所有学生信息
     * @return QList<QVariantMap> 学生列表
     */
    QList<QVariantMap> getAllStudents() const;

    /**
     * @brief 根据学号查询学生信息
     * @param id 学号
     * @return QList<QVariantMap> 学生列表
     */
    QList<QVariantMap> getStudentById(const QString& id) const;

    /**
     * @brief 更新学生信息
     * @param id 学号
     * @param name 姓名
     * @param gender 性别
     * @param birthday 生日
     * @param joinDate 加入日期
     * @param studyGoal 学习目标
     * @param progress 当前进度
     * @param photo 照片（二进制数据）
     * @return bool 操作是否成功
     */
    bool updateStudentInfo(const QString& id, const QString& name, const QString& gender,
                          const QString& birthday, const QString& joinDate, const QString& studyGoal,
                          const QString& progress, const QByteArray& photo = QByteArray());

    /**
     * @brief 删除学生信息
     * @param id 学号
     * @return bool 操作是否成功
     */
    bool deleteStudentInfo(const QString& id);

    // ==================== schedule 表操作 ====================
    /**
     * @brief 插入课程安排
     * @param date 日期
     * @param time 时间
     * @param courseName 课程名称
     * @return bool 操作是否成功
     */
    bool insertSchedule(const QString& date, const QString& time, const QString& courseName);

    /**
     * @brief 查询所有课程安排
     * @return QList<QVariantMap> 课程列表
     */
    QList<QVariantMap> getAllSchedules() const;

    /**
     * @brief 根据日期和时间查询课程安排
     * @param date 日期
     * @param time 时间
     * @return QList<QVariantMap> 课程列表
     */
    QList<QVariantMap> getScheduleByDateTime(const QString& date, const QString& time) const;

    /**
     * @brief 删除课程安排
     * @param date 日期
     * @param time 时间
     * @return bool 操作是否成功
     */
    bool deleteSchedule(const QString& date, const QString& time);

    // ==================== financialRecords 表操作 ====================
    /**
     * @brief 插入财务记录
     * @param studentId 学生学号
     * @param paymentDate 付款日期
     * @param amount 金额（字符串格式，避免精度丢失）
     * @param paymentType 付款类型
     * @param notes 备注
     * @return bool 操作是否成功
     */
    bool insertFinancialRecord(const QString& studentId, const QString& paymentDate, const QString& amount,
                              const QString& paymentType, const QString& notes = QString());

    /**
     * @brief 查询所有财务记录
     * @return QList<QVariantMap> 财务记录列表
     */
    QList<QVariantMap> getAllFinancialRecords() const;

    /**
     * @brief 根据学生学号查询财务记录
     * @param studentId 学生学号
     * @return QList<QVariantMap> 财务记录列表
     */
    QList<QVariantMap> getFinancialRecordsByStudentId(const QString& studentId) const;

    /**
     * @brief 根据ID删除财务记录
     * @param id 记录ID
     * @return bool 操作是否成功
     */
    bool deleteFinancialRecord(int id);

    // ==================== honorWall 表操作 ====================
    /**
     * @brief 插入荣誉墙记录
     * @param imageData 图片数据
     * @param description 描述
     * @return bool 操作是否成功
     */
    bool insertHonorWall(const QByteArray& imageData, const QString& description = QString());

    /**
     * @brief 查询所有荣誉墙记录
     * @return QList<QVariantMap> 荣誉墙列表
     */
    QList<QVariantMap> getAllHonorWall() const;

    /**
     * @brief 根据ID删除荣誉墙记录
     * @param id 记录ID
     * @return bool 操作是否成功
     */
    bool deleteHonorWall(int id);

    // ==================== users 表操作 ====================
    /**
     * @brief 插入用户
     * @param username 用户名
     * @param password 密码
     * @return bool 操作是否成功
     */
    bool insertUser(const QString& username, const QString& password);

    /**
     * @brief 查询所有用户
     * @return QList<QVariantMap> 用户列表
     */
    QList<QVariantMap> getAllUsers() const;

    /**
     * @brief 根据用户名查询用户
     * @param username 用户名
     * @return QList<QVariantMap> 用户列表
     */
    QList<QVariantMap> getUserByUsername(const QString& username) const;

    /**
     * @brief 删除用户
     * @param id 用户ID
     * @return bool 操作是否成功
     */
    bool deleteUser(int id);

private:
    /**
     * @brief 构造函数（私有化）
     * @param parent 父对象
     */
    explicit DataBaseManager(QObject *parent = nullptr);

    /**
     * @brief 初始化数据库
     * @return bool 初始化是否成功
     */
    bool initDatabase();

    /**
     * @brief 创建表结构
     * @return bool 建表是否成功
     */
    bool createTables();

    /**
     * @brief 打印数据库错误信息
     * @param error 错误对象
     * @param operation 操作描述
     * @param funcName 调用函数名
     */
    void printError(const QSqlError& error, const QString& operation, const char* funcName) const;

    /**
     * @brief 将 QSqlQuery 结果转换为 QVariantMap 列表
     * @param query 查询对象
     * @return QList<QVariantMap> 结果列表
     */
    QList<QVariantMap> queryToList(QSqlQuery& query) const;

    QSqlDatabase m_database; ///< 数据库连接

    // 数据库配置常量
    const static QString DB_CONN_NAME;  ///< 数据库连接名
};

#endif // DATABASEMANAGER_H
