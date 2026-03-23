#include "databasemanager.h"

// 定义数据库配置常量
const QString DataBaseManager::DB_DIR = "C:/Users/36146/code/QTproject/EdycationSystem/data/";
const QString DataBaseManager::DB_NAME = "student_system.db";
const QString DataBaseManager::DB_CONN_NAME = "student_system_conn";

DataBaseManager::DataBaseManager(QObject *parent) : QObject(parent)
{
    initDatabase();
}

DataBaseManager::~DataBaseManager()
{
    if (m_database.isOpen()) {
        m_database.close();
        qDebug() << "数据库连接已关闭";
    }
}

bool DataBaseManager::isConnected() const
{
    return m_database.isOpen();
}

QSqlDatabase &DataBaseManager::getQSqlDatabase()
{
    return m_database;
}

bool DataBaseManager::initDatabase()
{
    // 创建数据目录
    QDir dir(DB_DIR);
    if (!dir.exists()) {
        if (!dir.mkpath(DB_DIR)) {
            qDebug() << "创建数据目录失败：" << DB_DIR;
            return false;
        }
    }

    // 数据库文件路径
    QString dbPath = DB_DIR + DB_NAME;

    // 移除旧连接
    if (QSqlDatabase::contains(DB_CONN_NAME)) {
        QSqlDatabase::removeDatabase(DB_CONN_NAME);
    }

    // 建立数据库连接（使用自定义连接名）
    m_database = QSqlDatabase::addDatabase("QSQLITE", DB_CONN_NAME);
    m_database.setDatabaseName(dbPath);

    if (!m_database.open()) {
        printError(m_database.lastError(), "数据库连接", __func__);
        return false;
    }

    qDebug() << "数据库连接成功：" << dbPath;

    // 创建表结构
    return createTables();
}

bool DataBaseManager::createTables()
{
    QSqlQuery query(m_database);

    // 创建 studentInfo 表
    QString createStudentTable = R"(
        CREATE TABLE IF NOT EXISTS studentInfo (
            id TEXT PRIMARY KEY,
            name TEXT NOT NULL,
            gender TEXT,
            birthday TEXT,
            join_date TEXT,
            study_goal TEXT,
            progress TEXT,
            photo BLOB
        );
    )";

    if (!query.exec(createStudentTable)) {
        printError(query.lastError(), "创建 studentInfo 表", __func__);
        return false;
    }

    // 创建 schedule 表（联合主键）
    QString createScheduleTable = R"(
        CREATE TABLE IF NOT EXISTS schedule (
            date TEXT,
            time TEXT,
            course_name TEXT,
            PRIMARY KEY (date, time)
        );
    )";

    if (!query.exec(createScheduleTable)) {
        printError(query.lastError(), "创建 schedule 表", __func__);
        return false;
    }

    // 创建 financialRecords 表
    QString createFinancialTable = R"(
        CREATE TABLE IF NOT EXISTS financialRecords (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            student_id TEXT NOT NULL,
            payment_date DATE NOT NULL,
            amount DECIMAL(10,2) NOT NULL,
            payment_type VARCHAR(50) NOT NULL,
            notes TEXT,
            FOREIGN KEY (student_id) REFERENCES studentInfo(id)
        );
    )";

    if (!query.exec(createFinancialTable)) {
        printError(query.lastError(), "创建 financialRecords 表", __func__);
        return false;
    }

    // 创建 honorWall 表
    QString createHonorTable = R"(
        CREATE TABLE IF NOT EXISTS honorWall (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            image_data BLOB NOT NULL,
            description TEXT,
            added_date DATETIME DEFAULT CURRENT_TIMESTAMP
        );
    )";

    if (!query.exec(createHonorTable)) {
        printError(query.lastError(), "创建 honorWall 表", __func__);
        return false;
    }

    // 创建 users 表
    QString createUserTable = R"(
        CREATE TABLE IF NOT EXISTS users (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            username TEXT NOT NULL UNIQUE,
            password TEXT NOT NULL
        );
    )";

    if (!query.exec(createUserTable)) {
        printError(query.lastError(), "创建 users 表", __func__);
        return false;
    }

    qDebug() << "所有表创建成功";
    return true;
}

void DataBaseManager::printError(const QSqlError& error, const QString& operation, const char* funcName) const
{
    qDebug() << "数据库操作失败";
    qDebug() << "  函数名：" << funcName;
    qDebug() << "  操作：" << operation;
    qDebug() << "  错误类型：" << error.type();
    qDebug() << "  错误代码：" << error.nativeErrorCode();
    qDebug() << "  错误信息：" << error.text();
}

QList<QVariantMap> DataBaseManager::queryToList(QSqlQuery& query) const
{
    QList<QVariantMap> result;
    while (query.next()) {
        QVariantMap row;
        QSqlRecord record = query.record();
        for (int i = 0; i < record.count(); ++i) {
            row[record.fieldName(i)] = query.value(i);
        }
        result.append(row);
    }
    return result;
}

// ==================== studentInfo 表操作 ====================
bool DataBaseManager::insertStudentInfo(const QString& id, const QString& name, const QString& gender,
                                      const QString& birthday, const QString& joinDate, const QString& studyGoal,
                                      const QString& progress, const QByteArray& photo)
{
    QSqlQuery query(m_database);
    query.prepare(R"(
        INSERT INTO studentInfo (id, name, gender, birthday, join_date, study_goal, progress, photo)
        VALUES (?, ?, ?, ?, ?, ?, ?, ?)
    )");

    query.addBindValue(id);
    query.addBindValue(name);
    query.addBindValue(gender);
    query.addBindValue(birthday);
    query.addBindValue(joinDate);
    query.addBindValue(studyGoal);
    query.addBindValue(progress);
    query.addBindValue(photo);

    if (!query.exec()) {
        printError(query.lastError(), "插入学生信息", __func__);
        return false;
    }

    return true;
}

QList<QVariantMap> DataBaseManager::getAllStudents() const
{
    QSqlQuery query(m_database);
    query.exec("SELECT * FROM studentInfo");
    return const_cast<DataBaseManager*>(this)->queryToList(query);
}

QList<QVariantMap> DataBaseManager::getStudentById(const QString& id) const
{
    QSqlQuery query(m_database);
    query.prepare("SELECT * FROM studentInfo WHERE id = ?");
    query.addBindValue(id);
    query.exec();
    return const_cast<DataBaseManager*>(this)->queryToList(query);
}

bool DataBaseManager::updateStudentInfo(const QString& id, const QString& name, const QString& gender,
                                      const QString& birthday, const QString& joinDate, const QString& studyGoal,
                                      const QString& progress, const QByteArray& photo)
{
    QSqlQuery query(m_database);
    query.prepare(R"(
        UPDATE studentInfo
        SET name = ?, gender = ?, birthday = ?, join_date = ?, study_goal = ?, progress = ?, photo = ?
        WHERE id = ?
    )");

    query.addBindValue(name);
    query.addBindValue(gender);
    query.addBindValue(birthday);
    query.addBindValue(joinDate);
    query.addBindValue(studyGoal);
    query.addBindValue(progress);
    query.addBindValue(photo);
    query.addBindValue(id);

    if (!query.exec()) {
        printError(query.lastError(), "更新学生信息", __func__);
        return false;
    }

    return true;
}

bool DataBaseManager::deleteStudentInfo(const QString& id)
{
    QSqlQuery query(m_database);
    query.prepare("DELETE FROM studentInfo WHERE id = ?");
    query.addBindValue(id);

    if (!query.exec()) {
        printError(query.lastError(), "删除学生信息", __func__);
        return false;
    }

    return true;
}

// ==================== schedule 表操作 ====================
bool DataBaseManager::insertSchedule(const QString& date, const QString& time, const QString& courseName)
{
    QSqlQuery query(m_database);
    query.prepare(R"(
        INSERT INTO schedule (date, time, course_name)
        VALUES (?, ?, ?)
    )");

    query.addBindValue(date);
    query.addBindValue(time);
    query.addBindValue(courseName);

    if (!query.exec()) {
        printError(query.lastError(), "插入课程安排", __func__);
        return false;
    }

    return true;
}

QList<QVariantMap> DataBaseManager::getAllSchedules() const
{
    QSqlQuery query(m_database);
    query.exec("SELECT * FROM schedule");
    return const_cast<DataBaseManager*>(this)->queryToList(query);
}

QList<QVariantMap> DataBaseManager::getScheduleByDateTime(const QString& date, const QString& time) const
{
    QSqlQuery query(m_database);
    query.prepare("SELECT * FROM schedule WHERE date = ? AND time = ?");
    query.addBindValue(date);
    query.addBindValue(time);
    query.exec();
    return const_cast<DataBaseManager*>(this)->queryToList(query);
}

bool DataBaseManager::deleteSchedule(const QString& date, const QString& time)
{
    QSqlQuery query(m_database);
    query.prepare("DELETE FROM schedule WHERE date = ? AND time = ?");
    query.addBindValue(date);
    query.addBindValue(time);

    if (!query.exec()) {
        printError(query.lastError(), "删除课程安排", __func__);
        return false;
    }

    return true;
}

// ==================== financialRecords 表操作 ====================
bool DataBaseManager::insertFinancialRecord(const QString& studentId, const QString& paymentDate, const QString& amount,
                                          const QString& paymentType, const QString& notes)
{
    QSqlQuery query(m_database);
    query.prepare(R"(
        INSERT INTO financialRecords (student_id, payment_date, amount, payment_type, notes)
        VALUES (?, ?, ?, ?, ?)
    )");

    query.addBindValue(studentId);
    query.addBindValue(paymentDate);
    query.addBindValue(amount);
    query.addBindValue(paymentType);
    query.addBindValue(notes);

    if (!query.exec()) {
        printError(query.lastError(), "插入财务记录", __func__);
        return false;
    }

    return true;
}

QList<QVariantMap> DataBaseManager::getAllFinancialRecords() const
{
    QSqlQuery query(m_database);
    query.exec("SELECT * FROM financialRecords");
    return const_cast<DataBaseManager*>(this)->queryToList(query);
}

QList<QVariantMap> DataBaseManager::getFinancialRecordsByStudentId(const QString& studentId) const
{
    QSqlQuery query(m_database);
    query.prepare("SELECT * FROM financialRecords WHERE student_id = ?");
    query.addBindValue(studentId);
    query.exec();
    return const_cast<DataBaseManager*>(this)->queryToList(query);
}

bool DataBaseManager::deleteFinancialRecord(int id)
{
    QSqlQuery query(m_database);
    query.prepare("DELETE FROM financialRecords WHERE id = ?");
    query.addBindValue(id);

    if (!query.exec()) {
        printError(query.lastError(), "删除财务记录", __func__);
        return false;
    }

    return true;
}

// ==================== honorWall 表操作 ====================
bool DataBaseManager::insertHonorWall(const QByteArray& imageData, const QString& description)
{
    QSqlQuery query(m_database);
    query.prepare(R"(
        INSERT INTO honorWall (image_data, description)
        VALUES (?, ?)
    )");

    query.addBindValue(imageData);
    query.addBindValue(description);

    if (!query.exec()) {
        printError(query.lastError(), "插入荣誉墙记录", __func__);
        return false;
    }

    return true;
}

QList<QVariantMap> DataBaseManager::getAllHonorWall() const
{
    QSqlQuery query(m_database);
    query.exec("SELECT * FROM honorWall");
    return const_cast<DataBaseManager*>(this)->queryToList(query);
}

bool DataBaseManager::deleteHonorWall(int id)
{
    QSqlQuery query(m_database);
    query.prepare("DELETE FROM honorWall WHERE id = ?");
    query.addBindValue(id);

    if (!query.exec()) {
        printError(query.lastError(), "删除荣誉墙记录", __func__);
        return false;
    }

    return true;
}

// ==================== users 表操作 ====================
bool DataBaseManager::insertUser(const QString& username, const QString& password)
{
    QSqlQuery query(m_database);
    query.prepare(R"(
        INSERT INTO users (username, password)
        VALUES (?, ?)
    )");

    query.addBindValue(username);
    query.addBindValue(password);

    if (!query.exec()) {
        printError(query.lastError(), "插入用户", __func__);
        return false;
    }

    return true;
}

QList<QVariantMap> DataBaseManager::getAllUsers() const
{
    QSqlQuery query(m_database);
    query.exec("SELECT * FROM users");
    return const_cast<DataBaseManager*>(this)->queryToList(query);
}

QList<QVariantMap> DataBaseManager::getUserByUsername(const QString& username) const
{
    QSqlQuery query(m_database);
    query.prepare("SELECT * FROM users WHERE username = ?");
    query.addBindValue(username);
    query.exec();
    return const_cast<DataBaseManager*>(this)->queryToList(query);
}

bool DataBaseManager::deleteUser(int id)
{
    QSqlQuery query(m_database);
    query.prepare("DELETE FROM users WHERE id = ?");
    query.addBindValue(id);

    if (!query.exec()) {
        printError(query.lastError(), "删除用户", __func__);
        return false;
    }

    return true;
}
