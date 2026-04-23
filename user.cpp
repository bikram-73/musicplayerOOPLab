#include "user.h"

User::User() : id(-1), username(""), email("") {}

User::User(int id, const QString& username) : id(id), username(username), email("") {}

int User::getId() const {
    return id;
}

QString User::getUsername() const {
    return username;
}

QString User::getEmail() const {
    return email;
}

void User::setId(int newId) {
    id = newId;
}

void User::setUsername(const QString& newUsername) {
    username = newUsername;
}

void User::setEmail(const QString& newEmail) {
    email = newEmail;
}
