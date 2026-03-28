#include "honorwallwidget.h"
#include "ui_honorwallwidget.h"

HonorWallWidget::HonorWallWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::HonorWallWidget)
{
    ui->setupUi(this);
}

HonorWallWidget::~HonorWallWidget()
{
    delete ui;
}
