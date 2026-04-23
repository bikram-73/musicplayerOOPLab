#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include "user.h"
#include "authmanager.h"

// LoginDialog provides a UI for logging in and registering
class LoginDialog : public QDialog {
    Q_OBJECT

public:
    explicit LoginDialog(QWidget* parent = nullptr);
    User getAuthenticatedUser() const;

private slots:
    void onPrimaryClicked();
    void onSecondaryClicked();

private:
    QLineEdit* txtUsername;
    QLineEdit* txtEmail;
    QLineEdit* txtPassword;
    QPushButton* btnPrimary;
    QPushButton* btnSecondary;
    QLabel* lblStatus;

    AuthManager authManager;
    User authenticatedUser;
    bool isLoginMode = true;
};

#endif // LOGINDIALOG_H
