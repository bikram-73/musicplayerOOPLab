#include "authmanager.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QCryptographicHash>

AuthManager::AuthManager() {
}

void AuthManager::initialize() {
    QSqlQuery query;
    // We create a users table if it doesn't exist.
    // For a simple authentication system, we store the hash of the password.
    if (!query.exec("CREATE TABLE IF NOT EXISTS users ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                    "username TEXT UNIQUE, "
                    "email TEXT, "
                    "password_hash TEXT)")) {
        qDebug() << "Failed to create users table:" << query.lastError().text();
    }
    
    // In case the DB was already created in the previous step
    query.exec("ALTER TABLE users ADD COLUMN email TEXT");
}

// Simple helper to hash passwords using SHA-256 for basic security
static QString hashPassword(const QString& password) {
    QByteArray hash = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256);
    return QString(hash.toHex());
}

bool AuthManager::login(const QString& identifier, const QString& password, User& outUser) {
    QSqlQuery query;
    query.prepare("SELECT id, username, email FROM users WHERE (username = :usr OR email = :usr) AND password_hash = :pwd");
    query.bindValue(":usr", identifier);
    query.bindValue(":pwd", hashPassword(password));
    
    if (query.exec() && query.next()) {
        outUser.setId(query.value(0).toInt());
        outUser.setUsername(query.value(1).toString());
        outUser.setEmail(query.value(2).toString());
        return true;
    }
    return false;
}

bool AuthManager::registerUser(const QString& username, const QString& email, const QString& password) {
    if (username.isEmpty() || password.isEmpty() || email.isEmpty()) {
        return false;
    }
    
    QSqlQuery query;
    query.prepare("INSERT INTO users (username, email, password_hash) VALUES (:usr, :eml, :pwd)");
    query.bindValue(":usr", username);
    query.bindValue(":eml", email);
    query.bindValue(":pwd", hashPassword(password));
    
    // Returns true if insertion was successful (e.g. username is unique)
    return query.exec();
}
