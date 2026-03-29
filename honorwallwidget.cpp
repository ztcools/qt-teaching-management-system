#include "honorwallwidget.h"
#include "ui_honorwallwidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QScrollArea>
#include <QGridLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QLabel>
#include <QPixmap>
#include <QSqlQuery>
#include <QSqlError>
#include "databasemanager.h"
#include <QImage>
#include <QBuffer>
#include <QDateTime>
#include <QInputDialog>

// ClickableLabel 类的实现
ClickableLabel::ClickableLabel(QWidget *parent) : QLabel(parent), m_id(-1)
{}

void ClickableLabel::setId(int id)
{
    m_id = id;
}

void ClickableLabel::mousePressEvent(QMouseEvent *event)
{
    QLabel::mousePressEvent(event);
    emit clicked(m_id, this);
}



HonorWallWidget::HonorWallWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::HonorWallWidget)
    , selectedImageId(-1)
    , selectedLabel(nullptr)
{
    ui->setupUi(this);
    setupUI();
    loadImageFromDatabase();
}

HonorWallWidget::~HonorWallWidget()
{
    delete ui;
}

void HonorWallWidget::setupUI()
{
    //主布局
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    //按钮布局
    QHBoxLayout* buttonLayout = new QHBoxLayout;
    //添加按钮
    addButton = new QPushButton("添加图片",this);
    connect(addButton,&QPushButton::clicked,this,&HonorWallWidget::addImage);
    buttonLayout->addWidget(addButton);
    //修改按钮
    modifyButton = new QPushButton("修改图片",this);
    connect(modifyButton,&QPushButton::clicked,this,&HonorWallWidget::modifyImage);
    buttonLayout->addWidget(modifyButton);
    //删除按钮
    deleteButton = new QPushButton("删除图片",this);
    connect(deleteButton,&QPushButton::clicked,this,&HonorWallWidget::deleteImage);
    buttonLayout->addWidget(deleteButton);
    mainLayout->addLayout(buttonLayout);
    //滚动区域
    scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    //内容区域
    contentWidget = new QWidget(scrollArea);
    gridLayout = new QGridLayout(contentWidget);
    contentWidget->setLayout(gridLayout);
    //设置滚动区域的内容
    scrollArea->setWidget(contentWidget);
    mainLayout->addWidget(scrollArea);
    //设置主布局
    setLayout(mainLayout);
}

void HonorWallWidget::addImage()
{
    // 打开文件对话框，选择图片
    QString fileName = QFileDialog::getOpenFileName(this, "选择图片", "", "图片文件 (*.jpg *.jpeg *.png *.bmp)");
    if (fileName.isEmpty()) {
        return;
    }

    // 加载图片
    QImage image(fileName);
    if (image.isNull()) {
        QMessageBox::warning(this, "错误", "无法加载图片");
        return;
    }

    // 转换图片为二进制数据
    QByteArray imageData;
    QBuffer buffer(&imageData);
    buffer.open(QIODevice::WriteOnly);
    image.save(&buffer, "JPG");
    buffer.close();

    // 获取图片描述
    QString description = QInputDialog::getText(this, "图片描述", "请输入图片描述：");

    // 插入数据到数据库
    if (DataBaseManager::instance().insertHonorWall(imageData, description)) {
        QMessageBox::information(this, "成功", "添加图片成功");
        loadImageFromDatabase();
    } else {
        QMessageBox::critical(this, "错误", "添加图片失败");
    }
}

void HonorWallWidget::deleteImage()
{
    if (selectedImageId == -1) {
        QMessageBox::warning(this, "提示", "请先选择要删除的图片");
        return;
    }

    if (QMessageBox::question(this, "确认", "确定要删除选中的图片吗？", QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes) {
        return;
    }

    if (DataBaseManager::instance().deleteHonorWall(selectedImageId)) {
        QMessageBox::information(this, "成功", "删除图片成功");
        loadImageFromDatabase();
    } else {
        QMessageBox::critical(this, "错误", "删除图片失败");
    }
}

void HonorWallWidget::modifyImage()
{
    if (selectedImageId == -1) {
        QMessageBox::warning(this, "提示", "请先选择要修改的图片");
        return;
    }

    // 打开文件对话框，选择新图片
    QString fileName = QFileDialog::getOpenFileName(this, "选择新图片", "", "图片文件 (*.jpg *.jpeg *.png *.bmp)");
    if (fileName.isEmpty()) {
        return;
    }

    // 加载图片
    QImage image(fileName);
    if (image.isNull()) {
        QMessageBox::warning(this, "错误", "无法加载图片");
        return;
    }

    // 转换图片为二进制数据
    QByteArray imageData;
    QBuffer buffer(&imageData);
    buffer.open(QIODevice::WriteOnly);
    image.save(&buffer, "JPG");
    buffer.close();

    // 获取新的图片描述
    QString description = QInputDialog::getText(this, "修改图片描述", "请输入新的图片描述：");

    // 由于数据库管理类没有提供更新荣誉墙记录的方法，我们直接执行 SQL 语句
    QSqlDatabase& db = DataBaseManager::instance().getQSqlDatabase();
    QSqlQuery query(db);
    query.prepare("UPDATE honorWall SET image_data = ?, description = ? WHERE id = ?");
    query.addBindValue(imageData);
    query.addBindValue(description);
    query.addBindValue(selectedImageId);

    if (query.exec()) {
        QMessageBox::information(this, "成功", "修改图片成功");
        loadImageFromDatabase();
    } else {
        QMessageBox::critical(this, "错误", "修改图片失败：" + query.lastError().text());
    }
}

void HonorWallWidget::loadImageFromDatabase()
{
    // 清空布局中的所有控件
    QLayoutItem *item;
    while ((item = gridLayout->takeAt(0)) != nullptr) {
        if (item->widget()) {
            delete item->widget();
        }
        delete item;
    }

    // 重置选中状态
    selectedImageId = -1;
    selectedLabel = nullptr;

    // 从数据库获取所有荣誉墙记录
    QList<QVariantMap> records = DataBaseManager::instance().getAllHonorWall();

    // 显示图片
    int row = 0;
    int col = 0;
    for (const QVariantMap& record : records) {
        int id = record["id"].toInt();
        QByteArray imageData = record["image_data"].toByteArray();
        QString description = record["description"].toString();

        // 创建图片标签
        ClickableLabel* imageLabel = new ClickableLabel();
        imageLabel->setId(id);
        QPixmap pixmap;
        pixmap.loadFromData(imageData);
        // 缩放图片到合适大小
        pixmap = pixmap.scaled(150, 150, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        imageLabel->setPixmap(pixmap);
        imageLabel->setAlignment(Qt::AlignCenter);
        imageLabel->setFrameShape(QFrame::Box);
        imageLabel->setFrameShadow(QFrame::Sunken);
        imageLabel->setCursor(Qt::PointingHandCursor);

        // 创建描述标签
        QLabel* descLabel = new QLabel(description);
        descLabel->setAlignment(Qt::AlignCenter);
        descLabel->setWordWrap(true);

        // 创建垂直布局
        QVBoxLayout* itemLayout = new QVBoxLayout();
        itemLayout->addWidget(imageLabel);
        itemLayout->addWidget(descLabel);

        // 创建容器 widget
        QWidget* itemWidget = new QWidget();
        itemWidget->setLayout(itemLayout);

        // 添加到网格布局
        gridLayout->addWidget(itemWidget, row, col);

        // 连接点击信号
        connect(imageLabel, &ClickableLabel::clicked, this, &HonorWallWidget::onImageClicked);

        // 调整行列
        col++;
        if (col >= 3) { // 每行显示 3 张图片
            col = 0;
            row++;
        }
    }
}

void HonorWallWidget::onImageClicked(int id, ClickableLabel* label)
{
    // 重置之前选中的图片样式
    if (selectedLabel) {
        selectedLabel->setFrameShape(QFrame::Box);
        selectedLabel->setFrameShadow(QFrame::Sunken);
        selectedLabel->setStyleSheet("");
    }

    // 设置当前选中的图片样式
    selectedImageId = id;
    selectedLabel = label;
    selectedLabel->setFrameShape(QFrame::Box);
    selectedLabel->setFrameShadow(QFrame::Raised);
    selectedLabel->setStyleSheet("border: 3px solid blue; background-color: #e6f7ff;");
}
