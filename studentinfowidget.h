#ifndef STUDENTINFOWIDGET_H
#define STUDENTINFOWIDGET_H

#include <QWidget>
#include <QByteArray>
#include <QMap>

namespace Ui {
class StudentInfoWidget;
}
class QGroupBox;
class StudentInfoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit StudentInfoWidget(QWidget *parent = nullptr);
    ~StudentInfoWidget();
    void refreshTable();

private slots:
    void on_btnAdd_clicked();

    void on_btnDeleteItem_clicked();

    void on_btnDeleteLine_clicked();

private:
    Ui::StudentInfoWidget *ui;
    const QMap<int, QString> COLUMN_FIELD_MAP = {
        {0, "id"},          // 第0列 → 数据库id字段
        {1, "name"},        // 第1列 → 数据库name字段
        {2, "gender"},      // 第2列 → 数据库gender字段
        {3, "birthday"},    // 第3列 → 数据库birthday字段
        {4, "join_date"},   // 第4列 → 数据库join_date字段
        {5, "study_goal"},  // 第5列 → 数据库study_goal字段
        {6, "progress"},    // 第6列 → 数据库progress字段
        {7, "photo"}        // 第7列 → 数据库photo字段（明确指定照片列索引）
    };
    QGroupBox* createPhotoGroup();
    QGroupBox* createFormGroup();
    QByteArray photoData;
    void handleDialogAccepted(QGroupBox* formGroup,QGroupBox* photoGroup);

};

#endif // STUDENTINFOWIDGET_H
