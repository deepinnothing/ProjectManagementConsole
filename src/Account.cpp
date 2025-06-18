#include "Account.h"
#include <iostream>
#include <conio.h>
#include <filesystem>
#include <fstream>
#include <chrono>
#include <thread>
#include <vector>
#include <windows.h>

#include "ConsoleEssentials.h"
using namespace ace;

Account::Account(std::string login, std::string password, std::string name, std::string surname)
{
    ID = 1;
    setLogin(login);
    setPassword(password);
    setName(name);
    setSurname(surname);
}

Account::Account(const Account& acc)
{
    this->ID = acc.ID;
    for (short i = 0; i < 21; i++)
    {
        this->login[i] = acc.login[i];
        this->password[i] = acc.password[i];
    }
    for (short i = 0; i < 41; i++)
    {
        this->name[i] = acc.name[i];
        this->surname[i] = acc.surname[i];
    }
}

bool Account::checkForbiddenSymbols_LoginPassword(std::string str)
{
    for (int i = 0; i < str.length(); i++)
        if (!((str[i] >= 'A' && str[i] <= 'Z') || (str[i] >= 'a' && str[i] <= 'z') || (str[i] >= '0' && str[i] <= '9')
            || str[i] == '-' || str[i] == '.' || str[i] == '_'))
            return false;
    return true;
}

bool Account::checkForbiddenSymbols_NameSurname(std::string str)
{
    for (int i = 0; i < str.length(); i++)
        if (!((str[i] >= 'A' && str[i] <= 'Z') || (str[i] >= 'a' && str[i] <= 'z') || (str[i] >= 'А' && str[i] <= 'Я')
            || (str[i] >= 'а' && str[i] <= 'я') || str[i] == '-'))
            return false;
    return true;
}

bool Account::logging()
{
    if (!std::filesystem::exists("Accounts")) return false;
    std::ifstream fin("Accounts", std::ios::binary);
    Account checkingAccount;
    while (fin.read(reinterpret_cast<char*>(&checkingAccount), sizeof(Account)))
        if (strcmp(checkingAccount.login, login) == 0 && strcmp(checkingAccount.password, password) == 0)
        {
            for (short i = 0; i < 41; i++)
            {
                this->name[i] = checkingAccount.name[i];
                this->surname[i] = checkingAccount.surname[i];
            }
            this->ID = checkingAccount.ID;
            return true;
        }
    return false;
}

bool Account::create()
{
    if (std::filesystem::exists("Accounts"))
    {
        // Checking for a user with the same login
        std::ifstream fin("Accounts", std::ios::binary);
        Account checkingAccount;
        while (fin.read(reinterpret_cast<char*>(&checkingAccount), sizeof(Account)))
            if (strcmp(checkingAccount.login, login) == 0) return false;
        fin.close();
    }

    system("cls");
    // Entering name
    SetConsoleTextAttribute(handle, WHITE << 4 | BLACK);
    std::cout << " Please, enter your name:    " << std::endl;
    SetConsoleTextAttribute(handle, BLACK << 4 | WHITE);
    std::cout << " Please, enter your surname: " << std::endl;
    if (!inputField([this](const std::string& str) { setName(str); }, 40, 29, 0, 2))
        return false;

    // Entering surname
    setConsoleCursorPositionAndColor(0, 0, BLACK << 4 | WHITE);
    std::cout << " Please, enter your name:    " << name << std::endl;
    SetConsoleTextAttribute(handle, WHITE << 4 | BLACK);
    std::cout << " Please, enter your surname: " << std::endl;
    if (!inputField([this](const std::string& str) { setSurname(str); }, 40, 29, 1, 2))
        return false;

    setConsoleCursorPositionAndColor(0, 0, BLACK << 4 | WHITE);
    std::cout << " Please, enter your name:    " << name << std::endl;
    std::cout << " Please, enter your surname: " << surname << std::endl;

    // Saving data to a file
    if (std::filesystem::exists("Accounts")) ID = std::filesystem::file_size("Accounts") / sizeof(Account) + 1;
    std::ofstream fout("Accounts", std::ios::binary | std::ios::app);
    fout.write(reinterpret_cast<char*>(this), sizeof(Account));
    //clearMessage(2);
    setConsoleCursorPositionAndColor(0, 2, BLACK << 4 | GREEN);
    std::cout << " Account successfully created." << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    system("cls");
    return true;
}

void Account::setName(const std::string& name)
{
    if (name.length() > 40) throw std::length_error("Name length cannot exceed 40 characters");
    if (name.length() < 2) throw std::length_error("Name must be at least 2 characters long");
    if (!checkForbiddenSymbols_NameSurname(name))
        throw std::invalid_argument(
            "Forbidden characters are present in the name. Make sure that the name contains only Latin or Cyrillic characters and the '-' symbol");
    name.copy(this->name, 40);
    this->name[name.length()] = '\0';
}

void Account::setSurname(const std::string& surname)
{
    if (surname.length() > 40) throw std::length_error("Surname length cannot exceed 40 characters");
    if (surname.length() < 2) throw std::length_error("Surname must be at least 2 characters long");
    if (!checkForbiddenSymbols_NameSurname(surname))
        throw std::invalid_argument(
            "Forbidden characters are present in the surname. Make sure that the surname contains only Latin or Cyrillic characters and the '-' symbol");
    surname.copy(this->surname, 40);
    this->surname[surname.length()] = '\0';
}

void Account::setLogin(const std::string& login)
{
    if (login.length() > 20) throw std::length_error("Login length cannot exceed 20 characters");
    if (login.length() < 4) throw std::length_error("Login must be at least 4 characters long");
    if (login[0] >= '0' && login[0] <= '9') throw std::invalid_argument("Login cannot start with a number");
    if (!checkForbiddenSymbols_LoginPassword(login))
        throw std::invalid_argument(
            "Forbidden characters are present in the login. Make sure that the login contains only Latin letters, numbers, and symbols '.', '-' and '_'");
    login.copy(this->login, 20);
    this->login[login.length()] = '\0';
}

void Account::setPassword(const std::string& password)
{
    if (password.length() > 20) throw std::length_error("Password length cannot exceed 20 characters");
    if (password.length() < 8) throw std::length_error("Password must be at least 8 characters long");
    if (!checkForbiddenSymbols_LoginPassword(password))
        throw std::invalid_argument(
            "Forbidden characters are present in the password. Make sure that the password contains only Latin letters, numbers, and symbols '.', '-' and '_'");

    std::string encryptedPassword = password;
    reverse(encryptedPassword.begin(), encryptedPassword.end());
    for (short i = 0; i < encryptedPassword.length(); i++) encryptedPassword[i] -= 5 * (1 + i % 2);
    encryptedPassword.copy(this->password, 20);
    this->password[encryptedPassword.length()] = '\0';
}

bool Account::loginPrompt()
{
    while (true)
    {
        SetConsoleTextAttribute(handle, BLACK << 4 | WHITE);
        system("cls");
        std::vector<std::string> menu;
        menu.emplace_back(" Sign in ");
        menu.emplace_back(" Sign up ");
        menu.emplace_back(" Quit ");

        short command = 0;
        if (!printMenu(menu, command) || command == 2) return false;
        SetConsoleTextAttribute(handle, BLACK << 4 | WHITE);
        system("cls");

        // Entering login
        SetConsoleTextAttribute(handle, WHITE << 4 | BLACK);
        std::cout << " Enter your login:    " << std::endl;
        SetConsoleTextAttribute(handle, BLACK << 4 | WHITE);
        std::cout << " Enter your password: " << std::endl;
        if (!inputField([this](const std::string& str) { setLogin(str); }, 20, 22, 0, 2))
            continue;

        system("cls");
        // Entering password
        setConsoleCursorPositionAndColor(0, 0, BLACK << 4 | WHITE);
        std::cout << " Enter your login:    " << login << std::endl;
        SetConsoleTextAttribute(handle, WHITE << 4 | BLACK);
        std::cout << " Enter your password: " << std::endl;
        if (!inputField([this](const std::string& str) { setPassword(str); },
            20, 22, 1, 2, false, true))
            continue;

        setConsoleCursorPositionAndColor(0, 0, BLACK << 4 | WHITE);
        std::cout << " Enter your login:    " << login << std::endl;
        std::cout << " Enter your password: ";
        for (short i = 0; i < std::strlen(password); i++) std::cout << '*';
        std::cout << std::endl;
        setConsoleCursorPositionAndColor(0, 2, BLACK << 4 | GREEN);

        // Performing actions according to the previously selected menu item
        switch (command)
        {
        case 0:
            {
                if (this->logging()) std::cout << " Hello, " << name << '!' << std::endl;
                else
                {
                    SetConsoleTextAttribute(handle, BLACK << 4 | RED);
                    std::cerr << " Incorrect login or password" << std::endl;
                    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
                    system("cls");
                    continue;
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(2000));
                system("cls");
                break;
            }
        case 1:
            if (create()) break;
            SetConsoleTextAttribute(handle, BLACK << 4 | RED);
            std::cout << " An account with that login already exists" << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(2000));
            system("cls");
            continue;
        }
        break;
    }
    return true;
}

std::shared_ptr<Account> Account::currentAccount = std::make_shared<Account>();
