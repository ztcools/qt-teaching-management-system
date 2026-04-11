#include "schedulewidget.h"
#include "ui_schedulewidget.h"
#include <QDate>
#include <QComboBox>
#include <QTableWidget>
#include <QLabel>
#include <QPushButton>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QOverload>
#include <QPair>
#include <QVector>
#include "databasemanager.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QMessageBox>
#include <QTimeEdit>
#include <QDialogButtonBox>
#include <QDialog>
#include <QFormLayout>

ScheduleWidget::ScheduleWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ScheduleWidget)
{
    ui->setupUi(this);
    setupUI();

    int currentYear = QDate::currentDate().year();
    int currentWeek = customWeekNumber(QDate::currentDate());
    yearComboBox->setCurrentText(QString::number(currentYear));
    weekComboBox->setCurrentText(QString("第 %1 周").arg(currentWeek));
    loadSchedule();
}

ScheduleWidget::~ScheduleWidget()
{
    delete ui;
}

int ScheduleWidget::customWeekNumber(const QDate &date)
{
    QDate startOfYear(date.year(),1,1);
    int dayOfWeek = startOfYear.dayOfWeek();
    int days = startOfYear.daysTo(date);
    int week = (days + dayOfWeek - 1) / 7 + 1;
    return week;
}

void ScheduleWidget::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    QHBoxLayout* dateLayout = new QHBoxLayout();
    yearComboBox = new QComboBox(this);
    weekComboBox = new QComboBox(this);
    int currentYear = QDate::currentDate().year();
    for(int year = 2025;year <= currentYear + 5;++year)
    {
        yearComboBox->addItem(QString::number(year),year);
    }
    for(int week = 1;week <= 52;++week)
    {
        weekComboBox->addItem(QString("第%1周").arg(week),week);
    }
    dateRangeLabel = new QLabel(this);
    //添加周导航按钮
    QPushButton* prevWeekBtn = new QPushButton("上一周",this);
    QPushButton* nextWeekBtn = new QPushButton("下一周",this);
    prevWeekBtn->setFixedWidth(200);
    nextWeekBtn->setFixedWidth(200);
    dateLayout->addWidget(new QLabel("年份",this));
    dateLayout->addWidget(yearComboBox);
    dateLayout->addWidget(new QLabel("周数",this));
    dateLayout->addWidget(weekComboBox);
    dateLayout->addWidget(dateRangeLabel);
    dateLayout->addStretch();
    tableWidget = new QTableWidget(this);
    tableWidget->setAlternatingRowColors(true);
    tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    setupTable();
    addButton = new QPushButton("添加课程",this);
    deleteButton = new QPushButton("删除课程",this);
    addButton->setFixedWidth(200);
    deleteButton->setFixedWidth(200);
    connect(yearComboBox,QOverload<int>::of(&QComboBox::currentIndexChanged),this,&ScheduleWidget::loadSchedule);
    connect(weekComboBox,QOverload<int>::of(&QComboBox::currentIndexChanged),this,&ScheduleWidget::loadSchedule);
    connect(deleteButton,&QPushButton::clicked,this,&ScheduleWidget::deleteCourse);
    connect(addButton,&QPushButton::clicked,this,&ScheduleWidget::addCourse);
    connect(prevWeekBtn,&QPushButton::clicked,this,&ScheduleWidget::showPrevWeek);
    connect(nextWeekBtn,&QPushButton::clicked,this,&ScheduleWidget::showNextWeek);
    connect(tableWidget,&QTableWidget::itemChanged,this,&ScheduleWidget::handleItemChanged);

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(prevWeekBtn);
    buttonLayout->addWidget(nextWeekBtn);
    buttonLayout->addWidget(addButton);
    buttonLayout->addWidget(deleteButton);
    buttonLayout->addStretch();
    mainLayout->addLayout(dateLayout);
    mainLayout->addWidget(tableWidget);
    mainLayout->addLayout(buttonLayout);
    setLayout(mainLayout);

}

void ScheduleWidget::loadSchedule()
{
    tableWidget->blockSignals(true);
    tableWidget->clearContents();
    int year = yearComboBox->currentData().toInt();
    int week = weekComboBox->currentData().toInt();
    QPair<QDate,QDate>weekRange = getWeekRange(year,week);
    QDate startDate = weekRange.first;
    QDate endDate = weekRange.second;
    dateRangeLabel->setText(startDate.toString("yyyy-MM-dd") + "到" + endDate.toString("yyyy-MM-dd"));
    QVector<QVector<QString>> course(7,QVector<QString>(times.count(),""));
    QSqlDatabase& db = DataBaseManager::instance().getQSqlDatabase();
    QSqlQuery query(db);
    query.prepare("SELECT date,time,course_name FROM schedule WHERE date BETWEEN ? AND ?");
    query.addBindValue(startDate.toString("yyyy-MM-dd"));
    query.addBindValue(endDate.toString("yyyy-MM-dd"));
    if(query.exec())
    {
        while(query.next())
        {
            QDate date = QDate::fromString(query.value(0).toString(),"yyyy-MM-dd");
            QString time = query.value(1).toString();
            int dayIndex = startDate.daysTo(date);
            int timeIndex = times.indexOf(time);
            if(dayIndex >= 0 && dayIndex < 7 && timeIndex != -1)
            {
                course[dayIndex][timeIndex] = query.value(2).toString();
            }
        }
    }
    for(int day = 0;day < 7;++day)
    {
        for(int time = 0;time < times.count();++time)
        {
            QTableWidgetItem* item = new QTableWidgetItem(course[day][time]);
            item->setTextAlignment(Qt::AlignCenter);
            tableWidget->setItem(day,time,item);
        }
    }
    tableWidget->blockSignals(false);
}

void ScheduleWidget::setupTable()
{
    QStringList days = {"星期一","星期二","星期三","星期四","星期五","星期六","星期日"};
    times = {"上午1","上午2","下午1","下午2","晚上1","晚上2",};
    tableWidget->setRowCount(days.count());
    tableWidget->setColumnCount(times.count());
    int year = yearComboBox->currentData().toInt();
    int week = weekComboBox->currentData().toInt();
    QPair<QDate,QDate> weekRange = getWeekRange(year,week);
    QDate startDate = weekRange.first;
    QStringList verticalHeaders;
    for(int i = 0;i < days.count();++i)
    {
        QDate currentDate = startDate.addDays(i);
        verticalHeaders.append(QString("%1\n%2").arg(days[i]).arg(currentDate.toString("MM-dd")));
    }
    tableWidget->setVerticalHeaderLabels(verticalHeaders);
    tableWidget->setHorizontalHeaderLabels(times);
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableWidget->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed);
}

QPair<QDate, QDate> ScheduleWidget::getWeekRange(int year, int week)
{
    QDate startOfYear(year, 1, 1);
    int dayOfWeek = startOfYear.dayOfWeek();
    int daysToFirstMonday = (8 - dayOfWeek) % 7;
    QDate firstMonday = startOfYear.addDays(daysToFirstMonday);
    QDate startOfWeek = firstMonday.addDays((week - 1) * 7);
    QDate endOfWeek = startOfWeek.addDays(6);
    return QPair<QDate, QDate>(startOfWeek, endOfWeek);
}

void ScheduleWidget::addCourse()
{
    int dayIndex = tableWidget->currentRow();
    int timeIndex = tableWidget->currentColumn();

    if(dayIndex == -1 || timeIndex == -1)
    {
        QMessageBox::warning(this,"错误","请先选择一个时间段！");
        return;
    }

    QDialog dialog(this);
    dialog.setWindowTitle("添加课程");
    QFormLayout layout(&dialog);

    QComboBox nameCombo;
    QSqlDatabase& db = DataBaseManager::instance().getQSqlDatabase();
    QSqlQuery query(db);
    query.prepare("SELECT name FROM studentInfo");
    if(query.exec()){
        while(query.next())
        {
            nameCombo.addItem(query.value(0).toString());
        }
    }

    QMap<int,QTime>timePresets = {
        {0,QTime(9,0)},
        {1,QTime(11,0)},
        {2,QTime(14,0)},
        {3,QTime(16,0)},
        {4,QTime(19,0)},
        {5,QTime(21,0)}
    };

    QTimeEdit timeEdit;
    timeEdit.setTime(timePresets.value(timeIndex));

    layout.addRow("学生姓名",&nameCombo);
    layout.addRow("课程时间",&timeEdit);

    QDialogButtonBox buttons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    buttons.button(QDialogButtonBox::Ok)->setText("确定");
    buttons.button(QDialogButtonBox::Cancel)->setText("取消");
    layout.addRow(&buttons);

    connect(&buttons,&QDialogButtonBox::accepted,&dialog,&QDialog::accept);
    connect(&buttons,&QDialogButtonBox::rejected,&dialog,&QDialog::rejected);

    if(dialog.exec() != QDialog::Accepted)
        return;

    QString courseName = QString("%1,%2")
                             .arg(nameCombo.currentText())
                             .arg(timeEdit.time().toString("HH:mm"));

    // 获取日期
    int year = yearComboBox->currentData().toInt();
    int week = weekComboBox->currentData().toInt();
    QPair<QDate,QDate> weekRange = getWeekRange(year, week);
    QDate currentDate = weekRange.first.addDays(dayIndex);

    // ✅【修复】定义 timeSlot
    QString timeSlot = times[timeIndex];

    // 检查是否已存在相同日期和时间的记录
    QSqlQuery checkQuery(db);
    checkQuery.prepare("SELECT * FROM schedule WHERE date = ? AND time = ?");
    checkQuery.addBindValue(currentDate.toString("yyyy-MM-dd"));
    checkQuery.addBindValue(timeSlot);
    
    if(checkQuery.exec() && checkQuery.next()) {
        // 已存在记录，提示用户是否覆盖
        int ret = QMessageBox::question(this, "提示", "该时间段已有课程，是否覆盖？",
                                      QMessageBox::Yes | QMessageBox::No);
        if(ret == QMessageBox::No) {
            return; // 用户取消操作
        }
        // 覆盖现有记录
        QSqlQuery updateQuery(db);
        updateQuery.prepare("UPDATE schedule SET course_name = ? WHERE date = ? AND time = ?");
        updateQuery.addBindValue(courseName);
        updateQuery.addBindValue(currentDate.toString("yyyy-MM-dd"));
        updateQuery.addBindValue(timeSlot);
        
        if(!updateQuery.exec())
        {
            QMessageBox::critical(this,"错误","更新失败："+updateQuery.lastError().text());
        }
        else
        {
            loadSchedule();
        }
    } else {
        // 不存在记录，插入新记录
        QSqlQuery insertQuery(db);
        insertQuery.prepare("INSERT INTO schedule (date,time,course_name) VALUES(?,?,?)");
        insertQuery.addBindValue(currentDate.toString("yyyy-MM-dd"));
        insertQuery.addBindValue(timeSlot);
        insertQuery.addBindValue(courseName);

        if(!insertQuery.exec())
        {
            QMessageBox::critical(this,"错误","添加失败："+insertQuery.lastError().text());
        }
        else
        {
            loadSchedule();
        }
    }
}

void ScheduleWidget::handleItemChanged(QTableWidgetItem *item)
{
    // 1. 获取位置
    int day = item->row();
    int timeSlot = item->column();
    QString newCourse = item->text().trimmed();

    // 2. 获取当前日期
    int year = yearComboBox->currentData().toInt();
    int week = weekComboBox->currentData().toInt();
    QPair<QDate, QDate> weekRange = getWeekRange(year, week);
    QDate date = weekRange.first.addDays(day);
    QString time = times[timeSlot];

    // 3. 数据库操作
    QSqlDatabase& db = DataBaseManager::instance().getQSqlDatabase();
    QSqlQuery query(db);

    if (newCourse.isEmpty()) {
        // 空内容 → 删除
        query.prepare("DELETE FROM schedule WHERE date = ? AND time = ?");
    } else {
        // ✅【修复】表名 + VALUES 正确
        query.prepare("INSERT OR REPLACE INTO schedule (date, time, course_name) VALUES (?,?,?)");
    }

    // 绑定参数
    query.addBindValue(date.toString("yyyy-MM-dd"));
    query.addBindValue(time);
    if (!newCourse.isEmpty()) {
        query.addBindValue(newCourse);
    }

    // 执行
    if (!query.exec()) {
        QMessageBox::critical(this, "错误", "操作失败：" + query.lastError().text());
        loadSchedule(); // 恢复
    }
}

void ScheduleWidget::deleteCourse()
{
    int dayIndex = tableWidget->currentRow();
    int timeIndex = tableWidget->currentColumn();

    if(dayIndex == -1 || timeIndex == -1)
    {
        QMessageBox::warning(this,"错误","请先选择一个时间段！");
        return;
    }

    int year = yearComboBox->currentData().toInt();
    int week = weekComboBox->currentData().toInt();
    QPair<QDate,QDate> weekRange = getWeekRange(year, week);
    QDate currentDate = weekRange.first.addDays(dayIndex);
    QString timeSlot = times[timeIndex];

    // ✅ 自定义弹窗按钮文字
    QMessageBox msgBox;
    msgBox.setWindowTitle("提示");
    msgBox.setText("确定要删除该课程吗？");
    msgBox.setIcon(QMessageBox::Question);

    // 添加中文按钮
    QPushButton *btnConfirm = msgBox.addButton("确认", QMessageBox::AcceptRole);
    QPushButton *btnCancel = msgBox.addButton("取消", QMessageBox::RejectRole);
    msgBox.setDefaultButton(btnCancel); // 默认选中取消

    msgBox.exec();

    if (msgBox.clickedButton() != btnConfirm) {
        return; // 不是确认就取消操作
    }

    // 数据库删除逻辑
    QSqlDatabase& db = DataBaseManager::instance().getQSqlDatabase();
    QSqlQuery query(db);
    query.prepare("DELETE FROM schedule WHERE date = ? AND time = ?");
    query.addBindValue(currentDate.toString("yyyy-MM-dd"));
    query.addBindValue(timeSlot);

    if(!query.exec())
    {
        QMessageBox::critical(this,"错误","删除失败："+query.lastError().text());
    }
    else
    {
        loadSchedule();
    }
}

void ScheduleWidget::showPrevWeek()
{
    int currentWeek = weekComboBox->currentData().toInt();
    int currentYear = yearComboBox->currentData().toInt();

    int newWeek = currentWeek - 1;
    int newYear = currentYear;

    if(newWeek < 1) {
        newWeek = 52;
        newYear = currentYear - 1;
    }

    // 查找新的年份和周数
    int yearIndex = yearComboBox->findData(newYear);
    if(yearIndex != -1) {
        yearComboBox->setCurrentIndex(yearIndex);
    }

    int weekIndex = weekComboBox->findData(newWeek);
    if(weekIndex != -1) {
        weekComboBox->setCurrentIndex(weekIndex);
    }
}

void ScheduleWidget::showNextWeek()
{
    int currentWeek = weekComboBox->currentData().toInt();
    int currentYear = yearComboBox->currentData().toInt();

    int newWeek = currentWeek + 1;
    int newYear = currentYear;

    if(newWeek > 52) {
        newWeek = 1;
        newYear = currentYear + 1;
    }

    // 查找新的年份和周数
    int yearIndex = yearComboBox->findData(newYear);
    if(yearIndex != -1) {
        yearComboBox->setCurrentIndex(yearIndex);
    }

    int weekIndex = weekComboBox->findData(newWeek);
    if(weekIndex != -1) {
        weekComboBox->setCurrentIndex(weekIndex);
    }
}

