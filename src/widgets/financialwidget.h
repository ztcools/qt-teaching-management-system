#ifndef FINANCIALWIDGET_H
#define FINANCIALWIDGET_H

#include <QWidget>

class QChartView;
class QPushButton;
class QDateView;
class QComboBox;
class QTableWidget;
class QDateEdit;

namespace Ui {
class FinancialWidget;
}

class FinancialWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FinancialWidget(QWidget *parent = nullptr);
    ~FinancialWidget();

private:
    void setupUI();
    void loadFinancialRecords();
    void addRecord();
    void deleteRecord();
    void editRecord();
    void updateChart();
    void updatePieChart();
    void populateStudentComboBox();
    Ui::FinancialWidget *ui;
    QChartView* pieChartView;
    QTableWidget* tableWidget;
    QComboBox* studentComboBox;
    QPushButton* addButton;
    QPushButton* deleteButton;
    QPushButton* editButton;
    QChartView* chartView;
    QDateEdit* startDateEdit;
    QDateEdit* endDateEdit;

};

#endif // FINANCIALWIDGET_H
