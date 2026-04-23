#include "mainwindow.h"
#include "logindialog.h"

#include <QApplication>
#include <QStyleFactory>
#include <QSqlDatabase>
#include <QSqlError>
#include <QDebug>
#include <QDir>

bool initializeDatabase() {
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    // Ensure we create/access the database in a consistent directory
    db.setDatabaseName(QDir::currentPath() + "/music_library.db");
    if (!db.open()) {
        qDebug() << "Error: Could not open database!" << db.lastError().text();
        return false;
    }
    return true;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setStyle(QStyleFactory::create("Fusion"));

    if (!initializeDatabase()) {
        return -1;
    }

    // Show authentication dialog
    LoginDialog loginDialog;
    if (loginDialog.exec() == QDialog::Accepted) {
        // User authenticated successfully, get user and start main app
        User user = loginDialog.getAuthenticatedUser();
        MainWindow w(user); // Pass user to main window
        w.show();
        return QApplication::exec();
    }
    
    return 0; // Exits if dialog is closed or login fails entirely
}
