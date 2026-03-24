#include "studentinfowidget.h"
#include "ui_studentinfowidget.h"
#include "databasemanager.h"
#include <QTableWidgetItem>
#include <QPixmap>
#include <QDialog>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QDateEdit>
#include <QLabel>
#include <QString>
#include <QFileDialog>
#include <QStandardPaths>
#include <QBuffer>
#include <QMessageBox>
#include <QIODevice>
#include <QPushButton>
#include <QMap>
#include "tabledelegates.h"

StudentInfoWidget::StudentInfoWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::StudentInfoWidget)
{
    ui->setupUi(this);
    ui->tableWidget->verticalHeader()->setDefaultSectionSize(100);

    // 性别列代理（第2列）
    ComboBoxDelegate* genderDelegate = new ComboBoxDelegate(this);
    genderDelegate->setItems(QStringList() << "男" << "女");
    ui->tableWidget->setItemDelegateForColumn(2, genderDelegate);

    // 进度列代理（第6列）
    ComboBoxDelegate* progressDelegate = new ComboBoxDelegate(this);
    progressDelegate->setItems(QStringList() << "0%" << "20%" << "40%" << "60%" << "80%" << "100%");
    ui->tableWidget->setItemDelegateForColumn(6, progressDelegate);

    // 日期列代理（第3、4列）
    ui->tableWidget->setItemDelegateForColumn(3, new DateEditDelegate(this));
    ui->tableWidget->setItemDelegateForColumn(4, new DateEditDelegate(this));

    // ========================
    // 图片列代理（第7列，照片列）
    // ========================
    ui->tableWidget->setItemDelegateForColumn(7, new ImageDelegate(this));
    //连接item信号
    connect(ui->tableWidget,&QTableWidget::itemChanged,this,&StudentInfoWidget::handleItemChanged);

    refreshTable();
}

StudentInfoWidget::~StudentInfoWidget()
{
    delete ui;
}

void StudentInfoWidget::refreshTable()
{
    // 1. 临时屏蔽信号，避免刷新时触发不必要的槽函数
    ui->tableWidget->blockSignals(true);
    // 2. 清空表格（先记下列数，避免多次调用columnCount()）
    const int colCount = ui->tableWidget->columnCount();
    ui->tableWidget->setRowCount(0);

    // 3. 获取数据库数据
    DataBaseManager& db = DataBaseManager::instance();
    const QList<QVariantMap> students = db.getAllStudents();

    // 4. 填充数据（优化循环逻辑）
    for (const QVariantMap& student : students) { // 范围for，更简洁
        int row = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(row);

        for (int col = 0; col < colCount; ++col) {
            QTableWidgetItem* item = new QTableWidgetItem();
            item->setTextAlignment(Qt::AlignCenter);

            // 方式1：用映射表（推荐，灵活）
            if (!COLUMN_FIELD_MAP.contains(col)) {
                ui->tableWidget->setItem(row, col, item);
                continue; // 无映射的列跳过
            }
            QString fieldName = COLUMN_FIELD_MAP[col];

            // 方式2：用列表（按顺序）
            // if (col >= COLUMN_FIELD_LIST.size()) continue;
            // QString fieldName = COLUMN_FIELD_LIST[col];

            // 处理不同字段
            if (fieldName == "photo") { // 按字段名判断，而非列索引
                QByteArray photoData = student[fieldName].toByteArray();
                if (!photoData.isEmpty()) {
                    QPixmap photo;
                    if (photo.loadFromData(photoData)) { // 增加加载判断
                        item->setData(Qt::DecorationRole, photo.scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation));
                        item->setData(Qt::UserRole, photoData);
                    }
                }
            } else {
                // 容错：如果student中没有该字段，显示空字符串
                item->setText(student.value(fieldName, "").toString());
            }

            ui->tableWidget->setItem(row, col, item);
        }
    }

    // 5. 恢复信号
    ui->tableWidget->blockSignals(false);
}

void StudentInfoWidget::on_btnAdd_clicked()
{
    photoData.clear();
    QDialog dlg(this);
    dlg.setWindowTitle(tr("添加学生信息"));
    dlg.setMinimumSize(600, 400);

    // 初始化对话框布局
    QVBoxLayout* mainLayout = new QVBoxLayout(&dlg);
    QHBoxLayout* contentLayout = new QHBoxLayout();
    mainLayout->addLayout(contentLayout);

    // 添加表单和照片区域
    QGroupBox* formGroup = createFormGroup();
    QGroupBox* photoGroup = createPhotoGroup();
    contentLayout->addWidget(formGroup, 1);
    contentLayout->addWidget(photoGroup, 1);

    // 添加按钮区域
    QHBoxLayout* btnLayout = new QHBoxLayout();
    QPushButton* btnConfirm = new QPushButton(tr("确认"));
    QPushButton* btnCancel = new QPushButton(tr("取消"));

    // 配置按钮
    btnConfirm->setFixedWidth(150);
    btnCancel->setFixedWidth(150);

    // 添加按钮到布局
    btnLayout->addStretch();
    btnLayout->addWidget(btnConfirm);
    btnLayout->addWidget(btnCancel);
    btnLayout->addStretch();

    // 连接按钮信号
    connect(btnConfirm, &QPushButton::clicked, &dlg, &QDialog::accept);
    connect(btnCancel, &QPushButton::clicked, &dlg, &QDialog::reject);

    mainLayout->addLayout(btnLayout);

    // 执行对话框
    if (dlg.exec() == QDialog::Accepted) {
        handleDialogAccepted(formGroup, photoGroup);
    }
}

QGroupBox *StudentInfoWidget::createPhotoGroup()
{
    QGroupBox* photoGroup = new QGroupBox(tr("照片上传"));
    QVBoxLayout* photoLayout = new QVBoxLayout(photoGroup);
    //初始化控件
    QLabel* lblPhotoPreview = new QLabel();
    QPushButton* btnSelectPhoto = new QPushButton(tr("选择照片"));
    //配置控件
    lblPhotoPreview->setAlignment(Qt::AlignCenter);
    lblPhotoPreview->setMinimumSize(200,200);
    btnSelectPhoto->setFixedSize(100,40);
    //添加控件到布局
    photoLayout->addWidget(lblPhotoPreview);
    photoLayout->addWidget(btnSelectPhoto,0,Qt::AlignHCenter);
    //连接照片选择功能
    connect(btnSelectPhoto,&QPushButton::clicked,[this,lblPhotoPreview](){
        QString fileName = QFileDialog::getOpenFileName(this,tr("选择学生照片"),QStandardPaths::writableLocation(QStandardPaths::PicturesLocation),tr("图片文件(*.png *.jpg *.jpeg)"));
        if(!fileName.isEmpty())
        {
            QPixmap pixmap(fileName);
            if(!pixmap.isNull())
            {
                pixmap = pixmap.scaled(170,170,Qt::KeepAspectRatio);
                lblPhotoPreview->setPixmap(pixmap);
                QBuffer buffer(&photoData);
                buffer.open(QIODevice::WriteOnly);
                pixmap.save(&buffer,"PNG");
            }
            else
            {
                QMessageBox::warning(this,tr("错误"),tr("无法加载图片文件"));
            }
        }
    });
    return photoGroup;
}

QGroupBox *StudentInfoWidget::createFormGroup()
{
    QGroupBox* formGroup = new QGroupBox(tr("基本信息"));
    QFormLayout* formLayout = new QFormLayout(formGroup);
    //初始化控件
    QLineEdit* idEdit = new QLineEdit();
    idEdit->setObjectName("idEdit");
    QLineEdit* nameEdit = new QLineEdit();
    nameEdit->setObjectName("nameEdit");
    QComboBox* genderCombo = new QComboBox();
    genderCombo->setObjectName("genderCombo");
    QDateEdit* birthdayEdit = new QDateEdit(QDate::currentDate());
    birthdayEdit->setObjectName("birthdayEdit");
    QDateEdit* joinDateEdit = new QDateEdit(QDate::currentDate());
    joinDateEdit->setObjectName("joinDateEdit");
    QLineEdit* goalEdit = new QLineEdit();
    goalEdit->setObjectName("goalEdit");
    QComboBox* progressCombo = new QComboBox();
    progressCombo->setObjectName("progressCombo");

    //配置控件
    genderCombo->addItems({tr("男"),tr("女")});
    progressCombo->addItems({tr("0%"),tr("20%"),tr("40%"),tr("60%"),tr("80%"),tr("100%")});
    birthdayEdit->setDisplayFormat("yyyy-MM-dd");
    joinDateEdit->setDisplayFormat("yyyy-MM-dd");
    birthdayEdit->setCalendarPopup(true);
    joinDateEdit->setCalendarPopup(true);
    //添加控件到表单
    formLayout->addRow(tr("编号："),idEdit);
    formLayout->addRow(tr("姓名："),nameEdit);
    formLayout->addRow(tr("性别："),genderCombo);
    formLayout->addRow(tr("出生日期："),birthdayEdit);
    formLayout->addRow(tr("入学日期："),joinDateEdit);
    formLayout->addRow(tr("学习目标："),goalEdit);
    formLayout->addRow(tr("当前进度："),progressCombo);
    return formGroup;
}

void StudentInfoWidget::handleDialogAccepted(QGroupBox *formGroup, QGroupBox *photoGroup)
{
    // 1. 从表单控件提取数据
    QLineEdit* idEdit = formGroup->findChild<QLineEdit*>("idEdit");
    QLineEdit* nameEdit = formGroup->findChild<QLineEdit*>("nameEdit");
    QComboBox* genderCombo = formGroup->findChild<QComboBox*>("genderCombo");
    QDateEdit* birthdayEdit = formGroup->findChild<QDateEdit*>("birthdayEdit");
    QDateEdit* joinDateEdit = formGroup->findChild<QDateEdit*>("joinDateEdit");
    QLineEdit* goalEdit = formGroup->findChild<QLineEdit*>("goalEdit");
    QComboBox* progressCombo = formGroup->findChild<QComboBox*>("progressCombo");
    //数据校验
    if(idEdit->text().isEmpty() || nameEdit->text().isEmpty())
    {
        QMessageBox::warning(this,tr("错误"),tr("学号和姓名不能为空"));
        return;
    }
    // 获取数据库连接
    QSqlDatabase& db = DataBaseManager::instance().getQSqlDatabase();
    if (!db.isOpen()) {
        QMessageBox::critical(this, tr("错误"), tr("数据库连接未打开"));
        return;
    }
    //检查学号唯一性
    QSqlQuery checkQuery(db);
    checkQuery.prepare("SELECT id FROM studentInfo WHERE id = ?");
    checkQuery.addBindValue(idEdit->text());
    if(checkQuery.exec() && checkQuery.next())
    {
        QMessageBox::warning(this,tr("错误"),tr("学号 %1 已存在").arg(idEdit->text()));
        return;
    }
    //插入数据 - 使用 DataBaseManager 提供的方法
    bool success = DataBaseManager::instance().insertStudentInfo(
        idEdit->text(),
        nameEdit->text(),
        genderCombo->currentText(),
        birthdayEdit->date().toString("yyyy-MM-dd"),
        joinDateEdit->date().toString("yyyy-MM-dd"),
        goalEdit->text(),
        progressCombo->currentText(),
        photoData
    );
    
    if(!success)
    {
        QMessageBox::critical(this,tr("错误"),tr("添加失败，请检查数据库连接"));
    }
    else
    {
        refreshTable();
        QMessageBox::information(this,tr("成功"),tr("已成功添加学生：%1").arg(nameEdit->text()));
    }

}

void StudentInfoWidget::handleItemChanged(QTableWidgetItem *item)
{
    // 1. 获取行、列
    int row = item->row();
    int col = item->column();

    // 2. 禁止修改学号（第0列）
    if (col == 0) {
        QMessageBox::warning(this, "警告", "学号是主键，不能修改！");
        refreshTable();
        return;
    }

    // 3. 获取原始学号（WHERE条件用）
    QString originalId = ui->tableWidget->item(row, 0)->text();
    if (originalId.isEmpty()) return;

    // 4. 数据库列名（必须和你表结构一致）
    QStringList columnNames = {
        "id", "name", "gender", "birthday", "join_date", "study_goal", "progress", "photo"
    };

    // 越界保护
    if (col < 0 || col >= columnNames.size()) return;

    QString columnName = columnNames[col];

    // 5. 获取数据库连接
    QSqlDatabase& db = DataBaseManager::instance().getQSqlDatabase();
    if (!db.isOpen()) {
        QMessageBox::critical(this, "错误", "数据库未连接");
        return;
    }

    // 6. 开启事务
    db.transaction();

    QSqlQuery updateQuery(db);
    QString sql = QString("UPDATE studentInfo SET %1 = ? WHERE id = ?").arg(columnName);

    updateQuery.prepare(sql);

    // ==============================
    // 图片列（最后一列）
    // ==============================
    if (col == columnNames.size() - 1) {
        // ✅ 正确：从 model 取图片
        QModelIndex index = ui->tableWidget->model()->index(row, col);
        QByteArray imgData = index.data(Qt::UserRole).toByteArray();
        updateQuery.addBindValue(imgData);
    }
    // ==============================
    // 普通列
    // ==============================
    else {
        updateQuery.addBindValue(item->text().trimmed());
    }

    // WHERE id = ?
    updateQuery.addBindValue(originalId);

    // ==============================
    // 执行并判断
    // ==============================
    if (!updateQuery.exec()) {
        db.rollback();
        refreshTable();
        QMessageBox::critical(this, "更新失败",
                              "数据库错误：" + updateQuery.lastError().text());
        return;
    }

    // 提交
    db.commit();
}


void StudentInfoWidget::on_btnDeleteItem_clicked()
{
    auto selected = ui->tableWidget->selectedItems();
    if(selected.isEmpty())
    {
        QMessageBox::warning(this,"警告","请选择要删除的单元格！");
        return;
    }
    QSqlDatabase &db = DataBaseManager::instance().getQSqlDatabase();
    db.transaction();
    foreach(QTableWidgetItem * item,selected)
    {
        int row = item->row();
        int col = item->column();
        QString id = ui->tableWidget->item(row,0)->text();
        const QStringList columns = {"id","name","gender","birthday","join_date","study_goal","progress","photo"};
        QSqlQuery query(db);
        query.prepare(QString("UPDATE studentInfo SET %1 = ? WHERE id = ?").arg(columns[col]));
        query.addBindValue("");
        query.addBindValue(id);
        if(!query.exec())
        {
            db.rollback();
            QMessageBox::critical(this,"错误","更新失败"+query.lastError().text());
            return;
        }
    }
    db.commit();
    refreshTable();
}


void StudentInfoWidget::on_btnDeleteLine_clicked()
{
    auto selected = ui->tableWidget->selectionModel()->selectedRows();
    if(selected.isEmpty())
    {
        QMessageBox::warning(this,"警告","请先选择要删除的行！");
        return ;
    }
    QSqlDatabase &db = DataBaseManager::instance().getQSqlDatabase();
    db.transaction();
    foreach(const QModelIndex &index,selected)
    {
        QString id = ui->tableWidget->item(index.row(),0)->text();
        DataBaseManager::instance().deleteStudentInfo(id);
    }
    db.commit();
    refreshTable();
}

