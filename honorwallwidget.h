#ifndef HONORWALLWIDGET_H
#define HONORWALLWIDGET_H

#include <QWidget>

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
};

#endif // HONORWALLWIDGET_H
