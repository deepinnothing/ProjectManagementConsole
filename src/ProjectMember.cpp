#include "ProjectMember.h"

#include <iomanip>
#include <ios>
#include <iosfwd>
#include <iostream>
#include <vector>
#include <windows.h>

#include "Account.h"
#include "ConsoleEssentials.h"
using namespace ace;

std::ostream& operator<<(std::ostream& stream, ProjectMember& projMem)
{
    stream << std::left << std::setw(10) << projMem.account->getID() << '|' << std::setw(20) << projMem.account->
        getLogin() << '|';
    if (projMem.account->getName().length() > 20)
        stream << std::setw(20) << projMem.account->getName().substr(17) + "..." << '|';
    else stream << std::setw(20) << projMem.account->getName() << '|';
    if (projMem.account->getSurname().length() > 20)
        std::cout << std::setw(20) << projMem.account->getSurname().substr(17) + "...";
    else stream << std::setw(20) << projMem.account->getSurname();
    stream << '|' << std::setw(20) << projMem.role;
    return stream;
}

bool ProjectMember::checkForbiddenSymbols_Role(std::string str)
{
    for (int i = 0; i < str.length(); i++)
        if (!((str[i] >= 'A' && str[i] <= 'Z') || (str[i] >= 'a' && str[i] <= 'z') || (str[i] >= 'А' && str[i] <= 'Я')
            || (str[i] >= 'а' && str[i] <= 'я') || str[i] == '-' || str[i] == '.' || str[i] == ' ' || str[i] == '/'))
            return false;
    return true;
}

void ProjectMember::setRole(std::string role)
{
    if (role.length() > 20) throw std::length_error("Role length cannot exceed 20 characters");
    if (role.length() < 4) throw std::length_error("Role must be at least 4 characters long");
    if (!checkForbiddenSymbols_Role(role))
        throw std::invalid_argument(
            "Forbidden characters are present in the role. Make sure that the role contains only Latin or Cyrillic characters, and symbols '.' and '-'");
    role.copy(this->role, 20);
    this->role[role.length()] = '\0';
}

void ProjectMember::changeRole()
{
    SetConsoleTextAttribute(handle, BLACK << 4 | WHITE);
    system("cls");
    std::cout << "Assign a role to the project member: " << std::endl;

    short roleChoice = 0;
    std::vector<std::string> roles;
    roles.emplace_back(" Project administrator");
    roles.emplace_back(" Client / Project designer");
    roles.emplace_back(" Project developer");
    roles.emplace_back(" Project tester");
    if (!printMenu(roles, roleChoice, 1)) return;

    switch (roleChoice)
    {
    case 0: setRole("Administrator");
        break;
    case 1: setRole("Client / Designer");
        break;
    case 2: setRole("Developer");
        break;
    case 3: setRole("Tester");
        break;
    }
}

std::shared_ptr<ProjectMember> ProjectMember::currentProjectMember = std::make_shared<ProjectMember>();
