#pragma once

#include <memory>
#include <string>

#include "ConsoleEssentials.h"
#include "DateFunctions.h"
#include "ProjectMember.h"
using namespace ace;

enum Priority { Low, Medium, High };

enum issueTypes { TASK, FEATURE, BUG };

class Feature;

class Issue
{
    friend Project;
    friend Feature;
    static unsigned issuesCount;
    std::shared_ptr<ProjectMember> createdBy;
    std::shared_ptr<ProjectMember> assignedTo;

protected:
    struct IssueData
    {
        unsigned ID;
        char name[81];
        char description[2001];
        char dateCreated[11];
        char dateResolved[11];
        short priority;
    } issueData;

    static unsigned lastID;

    void print1();
    void print2();

public:
    static unsigned getIssuesCount() { return issuesCount; }
    static unsigned getLastID() { return lastID; }
    static void setLastID(unsigned ID) { lastID = ID; }
    std::string getName() const { return issueData.name; }
    std::string getDescription() const { return issueData.description; }
    std::string getDateCreated() const { return issueData.dateCreated; }
    std::string getDateResolved() const { return issueData.dateResolved; }
    short getPriority() const { return issueData.priority; }
    std::shared_ptr<ProjectMember> getCreatedBy() { return createdBy; }
    std::shared_ptr<ProjectMember> getAssignedTo() { return assignedTo; }
    void setName(const std::string& name);
    void setDescription(const std::string& description);
    void setDateCreated(const std::string& dateCreated);
    // Automatically set the creation date as the current date
    void setDateCreatedAuto()
    {
        dateCurrent().copy(issueData.dateCreated, 11);
    }
    void setDateResolved(const std::string& dateResolved);
    // Automatically set the authorization date as the current date
    void setDateResolvedAuto()
    {
        dateCurrent().copy(issueData.dateResolved, 11);
    }
    void setPriority(short);
    void setCreatedBy(std::shared_ptr<ProjectMember> createdBy) { this->createdBy = createdBy; }

    void setAssignedTo(std::shared_ptr<ProjectMember> assignedTo)
    {
        this->assignedTo = assignedTo;
    }

    // Changing issue priority
    bool changePriority();
    bool changeName();
    bool inputName()
    {
        return inputField([this](const std::string& str) { setName(str); }, 80, 0, 1, 2);
    }
    bool changeDescription();
    bool inputDescription()
    {
        return inputField([this](const std::string& str) { setDescription(str); }, 2000, 0, 3, -1, true);
    }
    virtual short getType() = 0;
    virtual bool create() = 0;
    virtual bool change() = 0;
    virtual void print() = 0;
    virtual void showInfo(std::string status);

    Issue(std::string name = "Issue name", std::string description = "Issue description\nMaximum 2000 characters",
          int priority = Medium);

    Issue(const Issue& issue);

    ~Issue() { issuesCount--; }
};
