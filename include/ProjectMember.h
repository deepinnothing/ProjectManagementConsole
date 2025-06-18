#pragma once

#include <memory>
#include <string>
#include "Account.h"

class Project;

class ProjectMember
{
    friend Project;
    char role[21];
    std::shared_ptr<Account> account;
    bool checkForbiddenSymbols_Role(std::string);

public:
    friend std::ostream& operator<<(std::ostream& stream, ProjectMember& projMem);
    std::string getRole() { return role; }
    std::shared_ptr<Account> getAccount() { return account; }
    void setRole(std::string);
    void setAccount(std::shared_ptr<Account> account) { this->account = account; }
    void changeRole();

    ProjectMember(std::string role = "Administrator")
    {
        role.copy(this->role, 20);
        this->role[role.length()] = '\0';
    }

    ProjectMember(const ProjectMember& projMem)
    {
        for (short i = 0; i < 21; i++) this->role[i] = projMem.role[i];
        this->account = projMem.account;
    }

    static std::shared_ptr<ProjectMember> currentProjectMember;
};
