#ifndef USER_H
#define USER_H

#include <QString>

// Abstract Base Class to demonstrate OOP concepts in detail
class IUser {
public:
    virtual ~IUser() = default;
    
    // Pure virtual methods ensuring all derived classes implement them
    virtual int getId() const = 0;
    virtual QString getUsername() const = 0;
    virtual QString getEmail() const = 0;
    virtual void setUsername(const QString& username) = 0;
    virtual void setEmail(const QString& email) = 0;
};

// Concrete User class encapsulating user data
class User : public IUser {
private:
    int id;
    QString username;
    QString email;

public:
    User();
    User(int id, const QString& username);

    // Getters and Setters (Encapsulation)
    int getId() const override;
    QString getUsername() const override;
    QString getEmail() const override;
    
    void setId(int newId);
    void setUsername(const QString& newUsername) override;
    void setEmail(const QString& newEmail) override;
};

#endif // USER_H
