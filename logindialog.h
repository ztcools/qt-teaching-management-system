#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>

class QLineEdit;
class QPushButton;
class QCheckBox;

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();

private:
    Ui::LoginDialog *ui;

    QLineEdit* usernameLineEdit;
    QLineEdit* passwordLineEdit;
    QPushButton* loginButton;
    QPushButton* cancelButton;
    QCheckBox* rememberPasswordCheckBox;
    void checkAndCreateInitialUser();
    QString hashPassword(const QString& password);
    void on_loginButton_clicked();
    bool loadCredentials(QString& username, QString& password);
    void saveCredentials(const QString& username, const QString& password);

};

#endif // LOGINDIALOG_H
