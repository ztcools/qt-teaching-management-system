#include "logindialog.h"
#include "ui_logindialog.h"
#include "databasemanager.h"
#include "settings.h"
#include <QSqlQuery>
#include <QDialog>
#include <QLabel>
#include <QGridLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QCheckBox>
#include <QCloseEvent>
LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LoginDialog)
{
    checkAndCreateInitialUser();
    setWindowTitle("教学管理系统");
    setFixedSize(260, 220);
    //创建控件
    QLabel* usernameLabel = new QLabel("用户名", this);
    QLabel* passwordLabel = new QLabel("密码", this);
    usernameLineEdit = new QLineEdit(this);
    passwordLineEdit = new QLineEdit(this);
    passwordLineEdit->setEchoMode(QLineEdit::Password);
    loginButton = new QPushButton("登录", this);
    cancelButton = new QPushButton("取消", this);
    rememberPasswordCheckBox = new QCheckBox("记住密码", this);
    rememberPasswordCheckBox->setChecked(Settings::instance().getCacheEnabled());
    //布局
    QGridLayout* mainLayout = new QGridLayout(this);
    mainLayout->addWidget(usernameLabel, 0, 0);
    mainLayout->addWidget(usernameLineEdit, 0, 1);
    mainLayout->addWidget(passwordLabel, 1, 0);
    mainLayout->addWidget(passwordLineEdit, 1, 1);
    mainLayout->addWidget(rememberPasswordCheckBox, 2, 0, 1, 2);
    QHBoxLayout* buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(loginButton);
    buttonLayout->addWidget(cancelButton);
    mainLayout->addLayout(buttonLayout, 3, 0, 1, 2);
    setLayout(mainLayout);
    //连接信号与槽
    connect(loginButton, &QPushButton::clicked, this, &LoginDialog::on_loginButton_clicked);
    connect(cancelButton, &QPushButton::clicked, this, &LoginDialog::reject);
    //尝试加载缓存信息
    QString cacheUsername, cachePassword;
    if (loadCredentials(cacheUsername, cachePassword)) {
        usernameLineEdit->setText(cacheUsername);
        passwordLineEdit->setText(cachePassword);
    }
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::closeEvent(QCloseEvent *event)
{
    // 用户点击关闭按钮时，拒绝对话框（返回 QDialog::Rejected）
    reject();
    event->accept();
}

void LoginDialog::checkAndCreateInitialUser()
{
    const QString initialUsername = qEnvironmentVariable("ADMIN_USER", "admin");
    const QString initialPassword = qEnvironmentVariable("ADMIN_PASS", "");
    if (initialPassword.isEmpty()) {
        qInfo() << "ADMIN_PASS not set, skipping default user creation";
        return;
    }
    QSqlDatabase& db = DataBaseManager::instance().getQSqlDatabase();
    QSqlQuery query(db);
    // 修复 SQL 语句中的拼写错误
    if (!query.exec("SELECT COUNT(*) FROM users")) {
        qDebug() << "查询用户数量失败：" << query.lastError().text();
        return;
    }
    if(query.next() && query.value(0).toInt() == 0)
    {
        QString hashedInitialPassword = Settings::instance().encryptPassword(initialPassword);
        query.prepare("INSERT INTO users (username,password) VALUES (:username,:password)");
        query.bindValue(":username", initialUsername);
        query.bindValue(":password", hashedInitialPassword);
        if(!query.exec())
        {
            qDebug() << "插入初始用户失败：" << query.lastError().text();
        }
    }
}

void LoginDialog::on_loginButton_clicked()
{
    QString username = usernameLineEdit->text();
    QString password = passwordLineEdit->text();

    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "警告", "请输入用户名和密码");
        return;
    }

    // 对密码进行哈希处理
    // 如果启用了记住密码，且当前密码框的值与缓存的加密密码相同，说明是自动填充的
    // 直接使用缓存的加密密码进行验证，不需要再次加密
    QString hashedPassword;
    if (Settings::instance().getRememberPassword() &&
        Settings::instance().getCacheEnabled()) {
        QString cachedPassword = Settings::instance().getCachedPassword();
        if (!cachedPassword.isEmpty() && password == cachedPassword) {
            // 使用的是缓存的加密密码
            hashedPassword = password;
        } else {
            // 用户手动输入了新密码，需要加密
            hashedPassword = Settings::instance().encryptPassword(password);
        }
    } else {
        // 没有启用记住密码，对用户输入的密码进行加密
        hashedPassword = Settings::instance().encryptPassword(password);
    }

    // 查询数据库验证用户
    QSqlDatabase& db = DataBaseManager::instance().getQSqlDatabase();
    QSqlQuery query(db);
    query.prepare("SELECT * FROM users WHERE username = :username AND password = :password");
    query.bindValue(":username", username);
    query.bindValue(":password", hashedPassword);

    if (!query.exec()) {
        QMessageBox::critical(this, "错误", "登录失败：" + query.lastError().text());
        return;
    }

    if (query.next()) {
        // 登录成功
        // 保存登录信息（只有当用户手动输入密码时才更新缓存）
        if (Settings::instance().getRememberPassword() &&
            Settings::instance().getCacheEnabled()) {
            QString cachedPassword = Settings::instance().getCachedPassword();
            if (!cachedPassword.isEmpty() && password == cachedPassword) {
                // 使用的是缓存密码，不更新缓存（避免重复加密）
            } else {
                // 用户手动输入了新密码，更新缓存
                saveCredentials(username, password);
            }
        } else {
            // 没有启用记住密码，正常保存
            saveCredentials(username, password);
        }
        // 保存最后登录用户
        Settings::instance().setLastUser(username);
        // 接受对话框
        accept();
    } else {
        // 登录失败
        QMessageBox::warning(this, "警告", "用户名或密码错误");
    }
}

bool LoginDialog::loadCredentials(QString &username, QString &password)
{
    username = Settings::instance().getLastUser();
    if (!Settings::instance().getCacheEnabled()) {
        return false;
    }
    if (Settings::instance().getRememberPassword()) {
        password = Settings::instance().getCachedPassword();
    }

    return !username.isEmpty();
}

void LoginDialog::saveCredentials(const QString& username, const QString& password)
{
    Settings::instance().setLastUser(username);
    Settings::instance().setCacheEnabled(true);
    Settings::instance().setRememberPassword(rememberPasswordCheckBox->isChecked());
    if (rememberPasswordCheckBox->isChecked()) {
        Settings::instance().setCachedPassword(password);
    } else {
        Settings::instance().setCachedPassword("");
    }
}
