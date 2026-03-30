#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>
#include <QSize>

class Settings
{
public:
    static Settings& instance();
    
    // 数据库设置
    QString getDatabasePath() const;
    void setDatabasePath(const QString& path);
    
    //登录设置
    bool getCacheEnabled() const;
    void setCacheEnabled(bool enabled);
    QString getLastUser() const;
    void setLastUser(const QString& user);
    bool getRememberPassword() const;
    void setRememberPassword(bool remember);
    QString getCachedPassword() const;
    void setCachedPassword(const QString& password);
    
    // 窗口设置
    QSize getWindowSize() const;
    void setWindowSize(const QSize& size);
    
    // 主题设置
    QString getTheme() const;
    void setTheme(const QString& theme);
    
    // 其他设置
    bool getAutoSaveEnabled() const;
    void setAutoSaveEnabled(bool enabled);
    
    // 密码加密和解密
    QString encryptPassword(const QString& password) const;
    QString decryptPassword(const QString& encryptedPassword) const;
    
private:
    Settings();
    QSettings settings;
};

#endif // SETTINGS_H
