#include "mainwindow.h"

#include <QApplication>
#include "databasemanager.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // ========== 第一步：初始化数据库（主窗口创建前） ==========
    DataBaseManager& dbManager = DataBaseManager::instance(); // 触发单例初始化
    if (!dbManager.isConnected()) {
        // 数据库初始化失败，直接退出程序
        qCritical() << "数据库初始化失败，程序退出！";
        return -1;
    }
    qInfo() << "数据库初始化成功，启动主窗口...";

    MainWindow w;
    w.show();
    return a.exec();
}
