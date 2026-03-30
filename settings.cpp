#include "settings.h"
#include <QCoreApplication>
#include <QCryptographicHash>
#include <QByteArray>

Settings &Settings::instance()
{
    static Settings instance;
    return instance;
}

QString Settings::getDatabasePath() const
{
    QString defaultPath = QCoreApplication::applicationDirPath() + "/data/student_system.db";
    QString path = settings.value("Database/Path", defaultPath).toString();
    qDebug() << "getDatabasePath:" << path << "(default:" << defaultPath << ")";
    return path;
}

void Settings::setDatabasePath(const QString &path)
{
    settings.setValue("Database/Path", path);
}

bool Settings::getCacheEnabled() const
{
    return settings.value("Login/CacheEnabled", true).toBool();
}

void Settings::setCacheEnabled(bool enabled)
{
    settings.setValue("Login/CacheEnabled", enabled);
}

QString Settings::getLastUser() const
{
    return settings.value("Login/LastUser", "").toString();
}

void Settings::setLastUser(const QString &user)
{
    settings.setValue("Login/LastUser", user);
}

bool Settings::getRememberPassword() const
{
    return settings.value("Login/RememberPassword", false).toBool();
}

void Settings::setRememberPassword(bool remember)
{
    settings.setValue("Login/RememberPassword", remember);
}

QString Settings::getCachedPassword() const
{
    QString encryptedPassword = settings.value("Login/CachedPassword", "").toString();
    return decryptPassword(encryptedPassword);
}

void Settings::setCachedPassword(const QString &password)
{
    QString encryptedPassword = encryptPassword(password);
    settings.setValue("Login/CachedPassword", encryptedPassword);
}

QSize Settings::getWindowSize() const
{
    return settings.value("Window/Size", QSize(1024, 768)).toSize();
}

void Settings::setWindowSize(const QSize &size)
{
    settings.setValue("Window/Size", size);
}

QString Settings::getTheme() const
{
    return settings.value("Appearance/Theme", "light").toString();
}

void Settings::setTheme(const QString &theme)
{
    settings.setValue("Appearance/Theme", theme);
}

bool Settings::getAutoSaveEnabled() const
{
    return settings.value("General/AutoSaveEnabled", true).toBool();
}

void Settings::setAutoSaveEnabled(bool enabled)
{
    settings.setValue("General/AutoSaveEnabled", enabled);
}

Settings::Settings(): settings("config.ini", QSettings::IniFormat) {
    qDebug() << "Settings file path:" << settings.fileName();
}

QString Settings::encryptPassword(const QString& password) const
{
    // 使用 SHA256 加密密码
    QByteArray hash = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256);
    return hash.toHex();
}

QString Settings::decryptPassword(const QString& encryptedPassword) const
{
    // 由于我们使用的是哈希加密，无法解密，直接返回加密后的密码
    // 在验证密码时，我们会对用户输入的密码进行同样的哈希处理，然后比较哈希值
    return encryptedPassword;
}

