#ifndef AUTHMANAGER_H
#define AUTHMANAGER_H

#include <QString>
#include "user.h"

// AuthManager abstracts database interactions related to authentication
class AuthManager {
public:
    AuthManager();
    
    // Checks if the users table exists and creates it if it doesn't
    void initialize();

    // Validates credentials, returns true and populates user info if successful
    bool login(const QString& identifier, const QString& password, User& outUser);

    // Registers a new user, returns true if successful
    bool registerUser(const QString& username, const QString& email, const QString& password);
};

#endif // AUTHMANAGER_H
