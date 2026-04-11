#include "financialwidget.h"
#include "ui_financialwidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QChartView>
#include <QLabel>
#include <QComboBox>
#include <QDateEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QOverload>
#include <QSqlQuery>
#include <qheaderview.h>
#include <QMessageBox>
#include <QSqlError>
#include "databasemanager.h"
#include <QLineEdit>
#include <QDoubleValidator>
#include <QDialogButtonBox>
#include <QChart>
#include <QLineSeries>
#include <QDateTimeAxis>
#include <QValueAxis>
#include <QPieSeries>
#include <QPieSlice>
#include <QLegend>
#include <algorithm> // 添加算法头文件

FinancialWidget::FinancialWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::FinancialWidget)
{
    ui->setupUi(this);
    setupUI();
    populateStudentComboBox();
    loadFinancialRecords();
}

FinancialWidget::~FinancialWidget()
{
    delete ui;
}

void FinancialWidget::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    QHBoxLayout* topLayout = new QHBoxLayout();
    QHBoxLayout* middleLayout = new QHBoxLayout();
    // 只创建一次 chartView
    chartView = new QChartView();
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setMinimumHeight(200);
    mainLayout->addLayout(topLayout);
    mainLayout->addLayout(middleLayout,60);
    mainLayout->addWidget(chartView,40);
    //顶部筛选条件与按钮布局
    topLayout->addWidget(new QLabel("学生姓名：",this));
    studentComboBox = new QComboBox(this);
    topLayout->addWidget(studentComboBox);
    topLayout->addWidget(new QLabel("起始日期：",this));
    startDateEdit = new QDateEdit(QDate::currentDate().addMonths(-1));
    startDateEdit->setCalendarPopup(true);
    topLayout->addWidget(startDateEdit);

    topLayout->addWidget(new QLabel("结束日期：",this));
    endDateEdit = new QDateEdit(QDate::currentDate());
    endDateEdit->setCalendarPopup(true);
    topLayout->addWidget(endDateEdit);
    addButton = new QPushButton("添加");
    deleteButton = new QPushButton("删除");
    editButton = new QPushButton("修改");
    topLayout->addWidget(addButton);
    topLayout->addWidget(deleteButton);
    topLayout->addWidget(editButton);
    topLayout->addStretch();

    //主布局
    tableWidget = new QTableWidget();
    tableWidget->setFixedWidth(550);
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableWidget->setAlternatingRowColors(true);
    // 设置行高
    tableWidget->verticalHeader()->setDefaultSectionSize(40);
    QStringList header = QStringList() << "ID" << "学生名字" << "缴费日期" << "金额" << "支付类型" << "备注";
    tableWidget->setColumnCount(header.count());
    tableWidget->setHorizontalHeaderLabels(header);
    tableWidget->setColumnHidden(0,true);
    middleLayout->addWidget(tableWidget);
    pieChartView = new QChartView();
    middleLayout->addWidget(pieChartView);

    connect(addButton,&QPushButton::clicked,this,&FinancialWidget::addRecord);
    connect(deleteButton,&QPushButton::clicked,this,&FinancialWidget::deleteRecord);
    connect(editButton,&QPushButton::clicked,this,&FinancialWidget::editRecord);
    connect(studentComboBox,QOverload<int>::of(&QComboBox::currentIndexChanged),this,&FinancialWidget::loadFinancialRecords);
    connect(startDateEdit,&QDateEdit::dateChanged,this,&FinancialWidget::loadFinancialRecords);
    connect(endDateEdit,&QDateEdit::dateChanged,this,&FinancialWidget::loadFinancialRecords);

}

void FinancialWidget::loadFinancialRecords()
{
    // 清空表格原有数据，避免重复加载
    tableWidget->setRowCount(0);

    // 获取筛选条件
    QString studentId = studentComboBox->currentData().toString();
    QDate startDate = startDateEdit->date();
    QDate endDate = endDateEdit->date();

    // 规范获取数据库单例，和课程模块保持统一
    QSqlDatabase& db = DataBaseManager::instance().getQSqlDatabase();
    QSqlQuery query(db);

    // 预编译SQL语句，补全所有字段，语法规范，无注入风险
    QString sqlStr = "SELECT fr.id, s.name, fr.payment_date, fr.amount, fr.payment_type, fr.notes "
                     "FROM financialRecords fr "
                     "JOIN studentInfo s ON fr.student_id = s.id "
                     "WHERE fr.payment_date BETWEEN :startDate AND :endDate ";

    // 非全部学生时，追加学生筛选条件
    if (studentId != "-1") {
        sqlStr += "AND fr.student_id = :studentId";
    }

    // 预编译+绑定参数，避免字符串拼接出错
    if (!query.prepare(sqlStr)) {
        QMessageBox::critical(this, "查询失败", "SQL 语句准备失败：" + query.lastError().text());
        return;
    }
    
    query.bindValue(":startDate", startDate.toString("yyyy-MM-dd"));
    query.bindValue(":endDate", endDate.toString("yyyy-MM-dd"));
    if (studentId != "-1") {
        query.bindValue(":studentId", studentId);
    }

    // 执行SQL并判断是否成功，失败弹出提示
    if (!query.exec()) {
        QMessageBox::critical(this, "查询失败", "财务记录加载失败：\n" + query.lastError().text());
        return;
    }

    // 遍历查询结果，填充表格
    while (query.next()) {
        int row = tableWidget->rowCount();
        tableWidget->insertRow(row);
        // 填充 ID 列（隐藏）
        QTableWidgetItem* idItem = new QTableWidgetItem(query.value(0).toString());
        tableWidget->setItem(row, 0, idItem);
        // 填充其他可见列
        for (int col = 1; col < 6; ++col) {
            QTableWidgetItem* item = new QTableWidgetItem(query.value(col).toString());
            item->setTextAlignment(Qt::AlignCenter);
            // 设置项不可编辑，和setupUI设置保持一致
            item->setFlags(item->flags() & ~Qt::ItemIsEditable);
            tableWidget->setItem(row, col, item);
        }
    }


    // 表头文字居中
    tableWidget->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
    // 更新图表
    updateChart();
    updatePieChart();
}


void FinancialWidget::addRecord()
{
    // 创建添加记录的对话框
    QDialog dialog(this);
    dialog.setWindowTitle("添加财务记录");
    dialog.resize(400, 300);
    
    QVBoxLayout* layout = new QVBoxLayout(&dialog);
    
    // 学生选择
    QHBoxLayout* studentLayout = new QHBoxLayout();
    studentLayout->addWidget(new QLabel("学生姓名："));
    QComboBox* studentCombo = new QComboBox();
    
    // 填充学生列表
    QSqlDatabase& db = DataBaseManager::instance().getQSqlDatabase();
    QSqlQuery query(db);
    query.prepare("SELECT id, name FROM studentInfo");
    if (query.exec()) {
        while (query.next()) {
            studentCombo->addItem(query.value(1).toString(), query.value(0).toString());
        }
    }
    studentLayout->addWidget(studentCombo);
    layout->addLayout(studentLayout);
    
    // 缴费日期
    QHBoxLayout* dateLayout = new QHBoxLayout();
    dateLayout->addWidget(new QLabel("缴费日期："));
    QDateEdit* dateEdit = new QDateEdit(QDate::currentDate());
    dateEdit->setCalendarPopup(true);
    dateLayout->addWidget(dateEdit);
    layout->addLayout(dateLayout);
    
    // 金额
    QHBoxLayout* amountLayout = new QHBoxLayout();
    amountLayout->addWidget(new QLabel("金额："));
    QLineEdit* amountEdit = new QLineEdit();
    amountEdit->setValidator(new QDoubleValidator(0, 999999, 2, this));
    amountLayout->addWidget(amountEdit);
    layout->addLayout(amountLayout);
    
    // 支付类型
    QHBoxLayout* typeLayout = new QHBoxLayout();
    typeLayout->addWidget(new QLabel("支付类型："));
    QComboBox* typeCombo = new QComboBox();
    typeCombo->addItems({"现金", "微信", "支付宝", "银行卡"});
    typeLayout->addWidget(typeCombo);
    layout->addLayout(typeLayout);
    
    // 备注
    QHBoxLayout* notesLayout = new QHBoxLayout();
    notesLayout->addWidget(new QLabel("备注："));
    QLineEdit* notesEdit = new QLineEdit();
    notesLayout->addWidget(notesEdit);
    layout->addLayout(notesLayout);
    
    // 按钮
    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    buttonBox->button(QDialogButtonBox::Ok)->setText("确认");
    buttonBox->button(QDialogButtonBox::Cancel)->setText("取消");
    layout->addWidget(buttonBox);
    
    connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    
    if (dialog.exec() == QDialog::Accepted) {
        // 获取输入值
        QString studentId = studentCombo->currentData().toString();
        QString paymentDate = dateEdit->date().toString("yyyy-MM-dd");
        QString amount = amountEdit->text();
        QString paymentType = typeCombo->currentText();
        QString notes = notesEdit->text();
        
        // 验证输入
        if (studentId.isEmpty() || amount.isEmpty()) {
            QMessageBox::warning(this, "提示", "请填写学生姓名和金额");
            return;
        }
        
        // 插入数据
        QSqlQuery insertQuery(db);
        insertQuery.prepare("INSERT INTO financialRecords (student_id, payment_date, amount, payment_type, notes) VALUES (?, ?, ?, ?, ?)");
        insertQuery.addBindValue(studentId);
        insertQuery.addBindValue(paymentDate);
        insertQuery.addBindValue(amountEdit->text().toDouble());  // 转换为 double 类型
        insertQuery.addBindValue(paymentType);
        insertQuery.addBindValue(notes);
        
        if (insertQuery.exec()) {
            QMessageBox::information(this, "成功", "添加财务记录成功");
            loadFinancialRecords();
        } else {
            QMessageBox::critical(this, "错误", "添加财务记录失败：" + insertQuery.lastError().text());
        }
    }
}

void FinancialWidget::deleteRecord()
{
    // 获取选中的行
    int currentRow = tableWidget->currentRow();
    if (currentRow < 0) {
        QMessageBox::warning(this, "提示", "请选择要删除的记录");
        return;
    }
    
    // 获取记录ID
    QString recordId = tableWidget->item(currentRow, 0)->text();
    
    // 确认删除
    if (QMessageBox::question(this, "确认", "确定要删除选中的记录吗？", QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes) {
        return;
    }
    
    // 删除记录
    QSqlDatabase& db = DataBaseManager::instance().getQSqlDatabase();
    QSqlQuery query(db);
    query.prepare("DELETE FROM financialRecords WHERE id = ?");
    query.addBindValue(recordId);
    
    if (query.exec()) {
        QMessageBox::information(this, "成功", "删除财务记录成功");
        loadFinancialRecords();
    } else {
        QMessageBox::critical(this, "错误", "删除财务记录失败：" + query.lastError().text());
    }
}

void FinancialWidget::editRecord()
{
    // 获取选中的行
    int currentRow = tableWidget->currentRow();
    if (currentRow < 0) {
        QMessageBox::warning(this, "提示", "请选择要修改的记录");
        return;
    }
    
    // 获取记录信息
    QString recordId = tableWidget->item(currentRow, 0)->text();
    QString studentName = tableWidget->item(currentRow, 1)->text();
    QString paymentDate = tableWidget->item(currentRow, 2)->text();
    QString amount = tableWidget->item(currentRow, 3)->text();
    QString paymentType = tableWidget->item(currentRow, 4)->text();
    QString notes = tableWidget->item(currentRow, 5)->text();
    
    // 创建修改记录的对话框
    QDialog dialog(this);
    dialog.setWindowTitle("修改财务记录");
    dialog.resize(400, 300);
    
    QVBoxLayout* layout = new QVBoxLayout(&dialog);
    
    // 学生选择
    QHBoxLayout* studentLayout = new QHBoxLayout();
    studentLayout->addWidget(new QLabel("学生姓名："));
    QComboBox* studentCombo = new QComboBox();
    
    // 填充学生列表
    QSqlDatabase& db = DataBaseManager::instance().getQSqlDatabase();
    QSqlQuery query(db);
    query.prepare("SELECT id, name FROM studentInfo");
    if (query.exec()) {
        while (query.next()) {
            studentCombo->addItem(query.value(1).toString(), query.value(0).toString());
            if (query.value(1).toString() == studentName) {
                studentCombo->setCurrentIndex(studentCombo->count() - 1);
            }
        }
    }
    studentLayout->addWidget(studentCombo);
    layout->addLayout(studentLayout);
    
    // 缴费日期
    QHBoxLayout* dateLayout = new QHBoxLayout();
    dateLayout->addWidget(new QLabel("缴费日期："));
    QDateEdit* dateEdit = new QDateEdit(QDate::fromString(paymentDate, "yyyy-MM-dd"));
    dateEdit->setCalendarPopup(true);
    dateLayout->addWidget(dateEdit);
    layout->addLayout(dateLayout);
    
    // 金额
    QHBoxLayout* amountLayout = new QHBoxLayout();
    amountLayout->addWidget(new QLabel("金额："));
    QLineEdit* amountEdit = new QLineEdit(amount);
    amountEdit->setValidator(new QDoubleValidator(0, 999999, 2, this));
    amountLayout->addWidget(amountEdit);
    layout->addLayout(amountLayout);
    
    // 支付类型
    QHBoxLayout* typeLayout = new QHBoxLayout();
    typeLayout->addWidget(new QLabel("支付类型："));
    QComboBox* typeCombo = new QComboBox();
    QStringList paymentTypes = {"现金", "微信", "支付宝", "银行卡"};
    typeCombo->addItems(paymentTypes);
    typeCombo->setCurrentText(paymentType);
    typeLayout->addWidget(typeCombo);
    layout->addLayout(typeLayout);
    
    // 备注
    QHBoxLayout* notesLayout = new QHBoxLayout();
    notesLayout->addWidget(new QLabel("备注："));
    QLineEdit* notesEdit = new QLineEdit(notes);
    notesLayout->addWidget(notesEdit);
    layout->addLayout(notesLayout);
    
    // 按钮
    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    layout->addWidget(buttonBox);
    
    connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    
    if (dialog.exec() == QDialog::Accepted) {
        // 获取输入值
        QString studentId = studentCombo->currentData().toString();
        QString newPaymentDate = dateEdit->date().toString("yyyy-MM-dd");
        QString newAmount = amountEdit->text();
        QString newPaymentType = typeCombo->currentText();
        QString newNotes = notesEdit->text();
        
        // 验证输入
        if (studentId.isEmpty() || newAmount.isEmpty()) {
            QMessageBox::warning(this, "提示", "请填写学生姓名和金额");
            return;
        }
        
        // 更新数据
        QSqlQuery updateQuery(db);
        updateQuery.prepare("UPDATE financialRecords SET student_id = ?, payment_date = ?, amount = ?, payment_type = ?, notes = ? WHERE id = ?");
        updateQuery.addBindValue(studentId);
        updateQuery.addBindValue(newPaymentDate);
        updateQuery.addBindValue(amountEdit->text().toDouble());  // 转换为 double 类型
        updateQuery.addBindValue(newPaymentType);
        updateQuery.addBindValue(newNotes);
        updateQuery.addBindValue(recordId);
        
        if (updateQuery.exec()) {
            QMessageBox::information(this, "成功", "修改财务记录成功");
            loadFinancialRecords();
        } else {
            QMessageBox::critical(this, "错误", "修改财务记录失败：" + updateQuery.lastError().text());
        }
    }
}

void FinancialWidget::updateChart()
{
    // 创建折线图，显示一段时间内的收入趋势
    QChart* chart = new QChart();
    chart->setTitle("财务收入趋势");
    
    // 获取筛选条件
    QString studentId = studentComboBox->currentData().toString();
    QDate startDate = startDateEdit->date();
    QDate endDate = endDateEdit->date();
    
    // 查询数据
    QSqlDatabase& db = DataBaseManager::instance().getQSqlDatabase();
    QSqlQuery query(db);
    
    QString sqlStr = "SELECT payment_date, SUM(amount) as total FROM financialRecords WHERE payment_date BETWEEN :startDate AND :endDate ";
    if (studentId != "-1") {
        sqlStr += "AND student_id = :studentId ";
    }
    sqlStr += "GROUP BY payment_date ORDER BY payment_date";
    
    if (!query.prepare(sqlStr)) {
        qDebug() << "SQL 语句准备失败：" << query.lastError().text();
        return;
    }
    
    query.bindValue(":startDate", startDate.toString("yyyy-MM-dd"));
    query.bindValue(":endDate", endDate.toString("yyyy-MM-dd"));
    if (studentId != "-1") {
        query.bindValue(":studentId", studentId);
    }
    
    if (query.exec()) {
        // 准备数据
        QLineSeries* series = new QLineSeries();
        QDateTimeAxis* axisX = new QDateTimeAxis();
        axisX->setFormat("yyyy-MM-dd");
        axisX->setTitleText("日期");
        
        QValueAxis* axisY = new QValueAxis();
        axisY->setTitleText("金额");
        axisY->setLabelFormat("%.0f");
        
        while (query.next()) {
            QDate date = QDate::fromString(query.value(0).toString(), "yyyy-MM-dd");
            double total = query.value(1).toDouble();
            series->append(date.startOfDay().toMSecsSinceEpoch(), total);
        }
        
        if (series->count() > 0) {
            chart->addSeries(series);
            chart->addAxis(axisX, Qt::AlignBottom);
            chart->addAxis(axisY, Qt::AlignLeft);
            series->attachAxis(axisX);
            series->attachAxis(axisY);
        } else {
            chart->setTitle("财务收入趋势（无数据）");
        }
    }
    
    chartView->setChart(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chart->legend()->setVisible(false);
}

void FinancialWidget::updatePieChart()
{
    // 创建饼图，显示不同支付类型的比例
    QChart* chart = new QChart();
    chart->setTitle("支付类型分布");
    
    // 获取筛选条件
    QString studentId = studentComboBox->currentData().toString();
    QDate startDate = startDateEdit->date();
    QDate endDate = endDateEdit->date();
    
    // 查询数据
    QSqlDatabase& db = DataBaseManager::instance().getQSqlDatabase();
    QSqlQuery query(db);
    
    QString sqlStr = "SELECT payment_type, SUM(amount) as total FROM financialRecords WHERE payment_date BETWEEN :startDate AND :endDate ";
    if (studentId != "-1") {
        sqlStr += "AND student_id = :studentId ";
    }
    sqlStr += "GROUP BY payment_type";
    
    if (!query.prepare(sqlStr)) {
        qDebug() << "SQL 语句准备失败：" << query.lastError().text();
        return;
    }
    
    query.bindValue(":startDate", startDate.toString("yyyy-MM-dd"));
    query.bindValue(":endDate", endDate.toString("yyyy-MM-dd"));
    if (studentId != "-1") {
        query.bindValue(":studentId", studentId);
    }
    
    if (query.exec()) {
        // 准备数据
        QPieSeries* series = new QPieSeries();
        
        while (query.next()) {
            QString type = query.value(0).toString();
            double total = query.value(1).toDouble();
            series->append(type, total);
        }
        
        if (series->count() > 0) {
            // 添加百分比标签
            for (QPieSlice* slice : series->slices()) {
                slice->setLabel(QString("%1 (%2%)").arg(slice->label()).arg(QString::number(slice->percentage() * 100, 'f', 1)));
            }
            chart->addSeries(series);
        } else {
            chart->setTitle("支付类型分布（无数据）");
        }
    }
    // ========== 兼容 Qt 6.8.3 的正确写法 ==========
    QLegend* legend = chart->legend();
    legend->setAlignment(Qt::AlignLeft);        // 图例放左侧
    legend->setInteractive(false);
    legend->setBackgroundVisible(false);       // 隐藏背景
    // 调整图例位置，确保文字显示完全
    pieChartView->setMinimumWidth(300);  // 增加饼图视图宽度
    
    pieChartView->setChart(chart);
}

void FinancialWidget::populateStudentComboBox()
{
    // 清空下拉框原有数据
    studentComboBox->clear();
    // 添加默认选项：所有学生，隐藏data为-1，用于全选筛选
    studentComboBox->addItem("所有学生", QVariant("-1"));

    // 规范获取数据库单例，和项目其他模块保持一致
    QSqlDatabase& db = DataBaseManager::instance().getQSqlDatabase();
    QSqlQuery query(db);

    // 预编译查询语句，更安全，适配表名（确保和实际数据库表名一致）
    query.prepare("SELECT id, name FROM studentInfo");

    // 执行查询并判断是否成功
    if (!query.exec()) {
        QMessageBox::warning(this, "提示", "学生数据加载失败：\n" + query.lastError().text());
        return;
    }

    // 遍历结果，添加到下拉框
    while (query.next()) {
        QString id = query.value(0).toString();
        QString name = query.value(1).toString();
        // 显示姓名，隐藏id，完全符合筛选逻辑
        studentComboBox->addItem(name, QVariant(id));
    }
}

