#ifndef SETTINGSWIDGET_H
#define SETTINGSWIDGET_H

#include <QWidget>

class QLabel;
class QLineEdit;
class QPushButton;
class QCheckBox;
class QComboBox;
class QGridLayout;

namespace Ui {
class SettingsWidget;
}

class SettingsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsWidget(QWidget *parent = nullptr);
    ~SettingsWidget();

private slots:
    void on_saveButton_clicked();
    void on_browseButton_clicked();
    void onChangePasswordButtonClicked();

private:
    Ui::SettingsWidget *ui;
    
    // 控件
    QLineEdit* databasePathLineEdit;
    QPushButton* browseButton;
    QCheckBox* cacheEnabledCheckBox;
    QCheckBox* rememberPasswordCheckBox;
    QCheckBox* autoSaveEnabledCheckBox;
    QComboBox* themeComboBox;
    QPushButton* changePasswordButton;
    
    // 初始化 UI
    void setupUI();
    // 加载设置
    void loadSettings();
    // 更改密码
    void changePassword();
};

#endif // SETTINGSWIDGET_H
