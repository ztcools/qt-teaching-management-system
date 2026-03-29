#ifndef HONORWALLWIDGET_H
#define HONORWALLWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QMouseEvent>

class QPushButton;
class QGridLayout;
class QScrollArea;

// 自定义 QLabel 类，用于处理点击事件
class ClickableLabel : public QLabel
{
    Q_OBJECT
public:
    explicit ClickableLabel(QWidget *parent = nullptr);

signals:
    void clicked(int id, ClickableLabel* label);

public slots:
    void setId(int id);

protected:
    void mousePressEvent(QMouseEvent *event) override;

private:
    int m_id;
};

namespace Ui {
class HonorWallWidget;
}

class HonorWallWidget : public QWidget
{
    Q_OBJECT

public:
    explicit HonorWallWidget(QWidget *parent = nullptr);
    ~HonorWallWidget();

private:
    Ui::HonorWallWidget *ui;
    QPushButton* addButton;
    QPushButton* modifyButton;
    QPushButton* deleteButton;
    QScrollArea* scrollArea;
    QWidget* contentWidget;
    QGridLayout* gridLayout;
    int selectedImageId; // 当前选中的图片 ID
    ClickableLabel* selectedLabel; // 当前选中的图片标签

    void setupUI();
    void addImage();
    void deleteImage();
    void modifyImage();
    void loadImageFromDatabase();
    void onImageClicked(int id, ClickableLabel* label);
};

#endif // HONORWALLWIDGET_H
