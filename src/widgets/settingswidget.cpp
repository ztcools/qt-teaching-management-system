#include "settingswidget.h"
#include "ui_settingswidget.h"
#include "settings.h"
#include "databasemanager.h"
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QComboBox>
#include <QFileDialog>
#include <QMessageBox>
#include <QApplication>
#include <QInputDialog>
#include <QDialog>
#include <QFormLayout>

SettingsWidget::SettingsWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SettingsWidget)
{
    ui->setupUi(this);
    setupUI();
    loadSettings();
}

SettingsWidget::~SettingsWidget()
{
    delete ui;
}

void SettingsWidget::setupUI()
{
    setWindowTitle("系统设置");
    
    // 获取 UI 文件中定义的控件
    databasePathLineEdit = ui->lineEdit;
    browseButton = ui->pushButton;
    cacheEnabledCheckBox = ui->checkBox;
    rememberPasswordCheckBox = ui->checkBox_2;
    autoSaveEnabledCheckBox = ui->checkBox_3;
    themeComboBox = ui->comboBox;
    
    // 创建更改密码按钮
    changePasswordButton = new QPushButton("更改密码", this);
    
    // 添加到布局
    QGridLayout* layout = qobject_cast<QGridLayout*>(this->layout());
    if (layout) {
        layout->addWidget(changePasswordButton, 6, 0, 1, 3);
    }
    
    // 连接信号与槽
    connect(browseButton, &QPushButton::clicked, this, &SettingsWidget::on_browseButton_clicked);
    connect(ui->pushButton_2, &QPushButton::clicked, this, &SettingsWidget::on_saveButton_clicked);
    connect(changePasswordButton, &QPushButton::clicked, this, &SettingsWidget::onChangePasswordButtonClicked);
}

void SettingsWidget::loadSettings()
{
    // 加载设置
    QString dbPath = Settings::instance().getDatabasePath();
    qDebug() << "Loaded database path:" << dbPath;
    databasePathLineEdit->setText(dbPath);
    
    bool cacheEnabled = Settings::instance().getCacheEnabled();
    qDebug() << "Loaded cache enabled:" << cacheEnabled;
    cacheEnabledCheckBox->setChecked(cacheEnabled);
    
    bool rememberPassword = Settings::instance().getRememberPassword();
    qDebug() << "Loaded remember password:" << rememberPassword;
    rememberPasswordCheckBox->setChecked(rememberPassword);
    
    bool autoSaveEnabled = Settings::instance().getAutoSaveEnabled();
    qDebug() << "Loaded auto save enabled:" << autoSaveEnabled;
    autoSaveEnabledCheckBox->setChecked(autoSaveEnabled);
    
    // 加载主题设置
    QString theme = Settings::instance().getTheme();
    qDebug() << "Loaded theme:" << theme;
    if (theme == "dark") {
        themeComboBox->setCurrentIndex(1);
    } else {
        themeComboBox->setCurrentIndex(0);
    }
}

void SettingsWidget::on_browseButton_clicked()
{
    QString filePath = QFileDialog::getSaveFileName(this, "选择数据库文件", ".", "SQLite 数据库文件 (*.db)");
    if (!filePath.isEmpty()) {
        databasePathLineEdit->setText(filePath);
    }
}

void SettingsWidget::on_saveButton_clicked()
{
    // 保存设置
    QString dbPath = databasePathLineEdit->text();
    qDebug() << "Saving database path:" << dbPath;
    Settings::instance().setDatabasePath(dbPath);
    
    bool cacheEnabled = cacheEnabledCheckBox->isChecked();
    qDebug() << "Saving cache enabled:" << cacheEnabled;
    Settings::instance().setCacheEnabled(cacheEnabled);
    
    bool rememberPassword = rememberPasswordCheckBox->isChecked();
    qDebug() << "Saving remember password:" << rememberPassword;
    Settings::instance().setRememberPassword(rememberPassword);
    
    bool autoSaveEnabled = autoSaveEnabledCheckBox->isChecked();
    qDebug() << "Saving auto save enabled:" << autoSaveEnabled;
    Settings::instance().setAutoSaveEnabled(autoSaveEnabled);
    
    // 保存主题设置
    QString theme = (themeComboBox->currentIndex() == 1) ? "dark" : "light";
    qDebug() << "Saving theme:" << theme;
    Settings::instance().setTheme(theme);
    
    QMessageBox::information(this, "成功", "设置已保存");
}

void SettingsWidget::onChangePasswordButtonClicked()
{
    changePassword();
}

void SettingsWidget::changePassword()
{
    // 创建对话框
    QDialog* dialog = new QDialog(this);
    dialog->setWindowTitle("更改密码");
    dialog->setFixedSize(300, 200);
    
    // 创建控件
    QVBoxLayout* layout = new QVBoxLayout(dialog);
    
    QLabel* oldPasswordLabel = new QLabel("旧密码:", dialog);
    QLineEdit* oldPasswordEdit = new QLineEdit(dialog);
    oldPasswordEdit->setEchoMode(QLineEdit::Password);
    
    QLabel* newPasswordLabel = new QLabel("新密码:", dialog);
    QLineEdit* newPasswordEdit = new QLineEdit(dialog);
    newPasswordEdit->setEchoMode(QLineEdit::Password);
    
    QLabel* confirmPasswordLabel = new QLabel("确认新密码:", dialog);
    QLineEdit* confirmPasswordEdit = new QLineEdit(dialog);
    confirmPasswordEdit->setEchoMode(QLineEdit::Password);
    
    QPushButton* okButton = new QPushButton("确定", dialog);
    QPushButton* cancelButton = new QPushButton("取消", dialog);
    
    // 添加到布局
    layout->addWidget(oldPasswordLabel);
    layout->addWidget(oldPasswordEdit);
    layout->addWidget(newPasswordLabel);
    layout->addWidget(newPasswordEdit);
    layout->addWidget(confirmPasswordLabel);
    layout->addWidget(confirmPasswordEdit);
    
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);
    layout->addLayout(buttonLayout);
    
    // 连接信号与槽
    connect(okButton, &QPushButton::clicked, dialog, [this, dialog, oldPasswordEdit, newPasswordEdit, confirmPasswordEdit]() {
        QString oldPassword = oldPasswordEdit->text();
        QString newPassword = newPasswordEdit->text();
        QString confirmPassword = confirmPasswordEdit->text();
        
        // 验证输入
        if (oldPassword.isEmpty() || newPassword.isEmpty() || confirmPassword.isEmpty()) {
            QMessageBox::warning(dialog, "警告", "请填写所有字段");
            return;
        }
        
        if (newPassword != confirmPassword) {
            QMessageBox::warning(dialog, "警告", "两次输入的新密码不一致");
            return;
        }
        
        // 验证旧密码
        QSqlDatabase& db = DataBaseManager::instance().getQSqlDatabase();
        QSqlQuery query(db);
        query.prepare("SELECT * FROM users WHERE username = :username AND password = :password");
        query.bindValue(":username", Settings::instance().getLastUser());
        query.bindValue(":password", Settings::instance().encryptPassword(oldPassword));
        
        if (!query.exec()) {
            QMessageBox::critical(dialog, "错误", "验证失败：" + query.lastError().text());
            return;
        }
        
        if (!query.next()) {
            QMessageBox::warning(dialog, "警告", "旧密码错误");
            return;
        }
        
        // 更新密码
        query.prepare("UPDATE users SET password = :password WHERE username = :username");
        query.bindValue(":password", Settings::instance().encryptPassword(newPassword));
        query.bindValue(":username", Settings::instance().getLastUser());
        
        if (!query.exec()) {
            QMessageBox::critical(dialog, "错误", "更新密码失败：" + query.lastError().text());
            return;
        }
        
        QMessageBox::information(dialog, "成功", "密码已更改");
        dialog->accept();
    });
    
    connect(cancelButton, &QPushButton::clicked, dialog, &QDialog::reject);
    
    // 显示对话框
    if (dialog->exec() != QDialog::Accepted) {
        return;
    }
}
