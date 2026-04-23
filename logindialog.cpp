#include "logindialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QGraphicsDropShadowEffect>
#include <QRegularExpression>

LoginDialog::LoginDialog(QWidget* parent) : QDialog(parent) {
    setWindowTitle("Music Player - Account");
    resize(1000, 600); // Make it the size of the main window so it doesn't look like a popup
    // Optionally remove window flags to make it frameless if needed, but a standard window is fine.
    
    setStyleSheet("QDialog { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #1db954, stop:0.2 #121212, stop:1 #000000); color: #ffffff; }"
                  "QLabel { color: #ffffff; font-weight: bold; font-family: 'Inter', 'Helvetica', sans-serif; }"
                  "QLineEdit { background-color: #282828; color: #ffffff; padding: 14px; border-radius: 6px; border: 1px solid #3e3e3e; font-size: 14px; }"
                  "QLineEdit:focus { border: 1px solid #1db954; background-color: #333333; }"
                  "QPushButton#btnPrimary { background-color: #1ed760; color: black; padding: 12px; border-radius: 8px; font-weight: bold; font-size: 15px; border: 1px solid #1ed760; }"
                  "QPushButton#btnPrimary:hover { background-color: #1db954; border-color: #1db954; }"
                  "QPushButton#btnPrimary:pressed { background-color: #1aa34a; border-color: #1aa34a; }"
                  "QPushButton#btnSecondary { background-color: transparent; color: #ffffff; border: 2px solid #b3b3b3; padding: 12px; }"
                  "QPushButton#btnSecondary:hover { border: 2px solid #ffffff; color: #ffffff; }");

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // Center container to act as the "Login Box"
    QWidget* centerWidget = new QWidget(this);
    centerWidget->setObjectName("centerWidget");
    centerWidget->setFixedWidth(420);
    centerWidget->setStyleSheet("QWidget#centerWidget { background-color: #000000; border-radius: 12px; border: 1px solid #333333; }");
    
    // Add subtle shadow to the center widget
    QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect;
    shadow->setBlurRadius(20);
    shadow->setColor(QColor(0, 0, 0, 150));
    shadow->setOffset(0, 4);
    centerWidget->setGraphicsEffect(shadow);

    QVBoxLayout* boxLayout = new QVBoxLayout(centerWidget);
    boxLayout->setContentsMargins(40, 50, 40, 50);
    boxLayout->setSpacing(20);
    
    QLabel* titleLabel = new QLabel("Music Player", centerWidget);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 32px; margin-bottom: 25px; border: none; background: transparent;");
    boxLayout->addWidget(titleLabel);

    txtUsername = new QLineEdit(centerWidget);
    txtUsername->setPlaceholderText("Email or username");
    boxLayout->addWidget(txtUsername);

    txtEmail = new QLineEdit(centerWidget);
    txtEmail->setPlaceholderText("Email address");
    txtEmail->hide(); // Hidden by default in Login mode
    boxLayout->addWidget(txtEmail);

    txtPassword = new QLineEdit(centerWidget);
    txtPassword->setPlaceholderText("Password");
    txtPassword->setEchoMode(QLineEdit::Password);
    boxLayout->addWidget(txtPassword);

    btnPrimary = new QPushButton("Log In", centerWidget);
    btnPrimary->setObjectName("btnPrimary");
    boxLayout->addWidget(btnPrimary);

    QLabel* divider = new QLabel("─────────  OR  ─────────", centerWidget);
    divider->setAlignment(Qt::AlignCenter);
    divider->setStyleSheet("color: #b3b3b3; font-size: 13px; margin: 15px 0; border: none; background: transparent;");
    boxLayout->addWidget(divider);

    btnSecondary = new QPushButton("Sign up free", centerWidget);
    btnSecondary->setObjectName("btnSecondary");
    boxLayout->addWidget(btnSecondary);
    
    lblStatus = new QLabel("", centerWidget);
    lblStatus->setStyleSheet("color: #f15e6c; font-size: 13px; font-weight: normal; border: none; background: transparent;");
    lblStatus->setAlignment(Qt::AlignCenter);
    boxLayout->addWidget(lblStatus);

    // To center the box vertically and horizontally
    QHBoxLayout* hLayout = new QHBoxLayout();
    hLayout->addStretch();
    hLayout->addWidget(centerWidget);
    hLayout->addStretch();

    mainLayout->addStretch();
    mainLayout->addLayout(hLayout);
    mainLayout->addStretch();

    connect(btnPrimary, &QPushButton::clicked, this, &LoginDialog::onPrimaryClicked);
    connect(btnSecondary, &QPushButton::clicked, this, &LoginDialog::onSecondaryClicked);

    authManager.initialize(); // Ensure DB table exists
}

void LoginDialog::onPrimaryClicked() {
    lblStatus->clear();

    if (isLoginMode) {
        // Attempt Login Action
        QString identifier = txtUsername->text().trimmed(); 
        QString password = txtPassword->text();

        if (identifier.isEmpty() || password.isEmpty()) {
            lblStatus->setText("Please enter both username and password.");
            return;
        }

        if (authManager.login(identifier, password, authenticatedUser)) {
            accept(); 
        } else {
            lblStatus->setText("Incorrect username or password.");
        }
    } else {
        // Attempt Register Action
        QString username = txtUsername->text().trimmed();
        QString email = txtEmail->text().trimmed();
        QString password = txtPassword->text();

        if (username.isEmpty() || password.isEmpty() || email.isEmpty()) {
            lblStatus->setText("Please fill out username, email, and password.");
            return;
        }

        QRegularExpression emailRegex("^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\.[A-Za-z]{2,}$");
        if (!emailRegex.match(email).hasMatch()) {
            lblStatus->setText("Please enter a valid email address.");
            return;
        }

        if (authManager.registerUser(username, email, password)) {
            // Automatically log them in immediately!
            if (authManager.login(username, password, authenticatedUser)) {
                accept(); 
            }
        } else {
            lblStatus->setText("Registration failed. Username may already exist.");
        }
    }
}

void LoginDialog::onSecondaryClicked() {
    lblStatus->clear();

    if (isLoginMode) {
        // User clicked "Sign up free" - switch to Register Mode
        isLoginMode = false;
        txtEmail->show();
        txtUsername->setPlaceholderText("Choose a username");
        btnPrimary->setText("Confirm Sign Up");
        btnSecondary->setText("Already have an account? Log In");
        txtUsername->clear();
        txtPassword->clear();
    } else {
        // User clicked "Log In" - switch back to Login Mode
        isLoginMode = true;
        txtEmail->hide();
        txtUsername->setPlaceholderText("Email or username");
        btnPrimary->setText("Log In");
        btnSecondary->setText("Sign up free");
        txtUsername->clear();
        txtPassword->clear();
    }
}

User LoginDialog::getAuthenticatedUser() const {
    return authenticatedUser;
}
