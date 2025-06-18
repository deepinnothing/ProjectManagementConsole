#pragma once
#include <memory>
#include <string>

class Project;

class Account
{
    friend Project;
    unsigned ID;
    char login[21];
    char password[21];
    char name[41];
    char surname[41];

    // Checks login and password for forbidden characters
    bool checkForbiddenSymbols_LoginPassword(std::string);

    // Checks name and surname for forbidden characters
    bool checkForbiddenSymbols_NameSurname(std::string);

    // Checks login credentials and signs in the user
    bool logging();

    // Creates a new account
    bool create();

public:
    unsigned getID() const { return ID; }
    std::string getName() const { return name; }

    // Checks the name and sets it if there's no problem
    void setName(const std::string& name);

    std::string getSurname() const { return surname; }

    // Checks the surname and sets it if there's no problem
    void setSurname(const std::string& surname);

    std::string getLogin() const { return login; }

    // Checks the login (username) and sets it if there's no problem
    void setLogin(const std::string& login);

    // Checks the password and then encrypts and sets it if there's no problem
    void setPassword(const std::string& password);

    // Prompt a user to sign in to their account
    bool loginPrompt();

    Account(std::string login = "Login", std::string password = "Password", std::string name = "Name",
            std::string surname = "Surname");
    Account(const Account& acc);

    static std::shared_ptr<Account> currentAccount;
};
