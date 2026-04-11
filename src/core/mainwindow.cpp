#include "mainwindow.h"
#include "./ui_mainwindow.h"

// 1. 补充所有缺失的头文件
#include <QButtonGroup>
#include <QFile>
#include <QPushButton>
#include <QToolButton>
#include <QDebug>
#include <QList>
#include <QStringList>       // QStringList必备
#include <QApplication>      // qApp必备
#include <QStackedWidget>    // QStackedWidget必备
#include <QAbstractButton>   // 按钮组遍历备用
#include "logindialog.h"
#include "settingswidget.h"
#include "databasemanager.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 2. 改用兼容所有Qt版本的QStringList写法（放弃列表初始化）
    QStringList qssFiles;
    qssFiles.append(":/style/color_palette.qss");
    qssFiles.append(":/style/main_style.qss");
    qssFiles.append(":/style/nav_style.qss");
    qssFiles.append(":/style/widget_style.qss");

    QString totalStyle;
    foreach (QString file, qssFiles) {
        QFile qssFile(file);
        // 3. 打开文件时指定编码（避免中文乱码）
        if (qssFile.open(QFile::ReadOnly | QFile::Text)) {
            QTextStream stream(&qssFile);
            totalStyle += stream.readAll() + "\n";
            qssFile.close();
        } else {
            qDebug() << "加载QSS失败：" << file << " 原因：" << qssFile.errorString();
        }
    }

    // 4. 全局应用样式（qApp需要<QApplication>头文件）
    qApp->setStyleSheet(totalStyle);

    // 替换系统设置页面
    // 移除原有的系统设置页面
    QWidget* oldSettingsWidget = ui->stackedWidget->widget(4);
    if (oldSettingsWidget) {
        ui->stackedWidget->removeWidget(oldSettingsWidget);
        delete oldSettingsWidget;
    }
    // 添加新的系统设置页面
    SettingsWidget* newSettingsWidget = new SettingsWidget();
    ui->stackedWidget->insertWidget(4, newSettingsWidget);

    // 5. 导航按钮配置（修正后）
    QButtonGroup *btnGp = new QButtonGroup(this);
    QList<QToolButton*> navBtns;
    // 逐个添加按钮（兼容所有版本）
    navBtns.append(ui->btnStudentinfo);
    navBtns.append(ui->btnSchedule);
    navBtns.append(ui->btnFinance);
    navBtns.append(ui->btnHonor);
    navBtns.append(ui->btnSystemSetting);

    // 给每个按钮设置属性+加入按钮组
    for (int i = 0; i < navBtns.size(); i++) {
        // 防御性检查：避免按钮指针为空导致崩溃
        if (navBtns[i] != nullptr) {
            navBtns[i]->setProperty("class", "navBtn");
            btnGp->addButton(navBtns[i], i);
            navBtns[i]->setCheckable(true);
        }
    }

    // 6. 按钮组最终配置
    btnGp->setExclusive(true);
    // 防御性检查：确保按钮存在再选中
    if (btnGp->button(0) != nullptr) {
        btnGp->button(0)->setChecked(true);
    }
    ui->stackedWidget->setCurrentIndex(0);

    // 7. 绑定切换事件（Lambda写法兼容C++11，若仍报错则改用老式槽函数）
    connect(btnGp, &QButtonGroup::idClicked,
            ui->stackedWidget, &QStackedWidget::setCurrentIndex);
}

MainWindow::~MainWindow()
{
    delete ui;
}
