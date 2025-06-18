#include "Issue.h"

#include <conio.h>
#include <iomanip>
#include <iostream>
#include <thread>
#include <vector>
#include <windows.h>

#include "ProjectMember.h"

void Issue::print1()
{
    if (getName().length() > 26) std::cout << std::setw(26) << getName().substr(0, 23) + "..." << '|';
    else std::cout << std::setw(26) << issueData.name << '|';
    if (issueData.priority == Low) std::cout << std::setw(8) << "Low" << '|';
    else if (issueData.priority == Medium) std::cout << std::setw(8) << "Medium" << '|';
    else if (issueData.priority == High) std::cout << std::setw(8) << "High" << '|';
    std::cout << std::setw(13) << ' ' + getDateCreated() << '|' << std::setw(12) << ' ' + getDateResolved() << '|';
}

void Issue::print2()
{
    std::string fullName;
    if (getCreatedBy() != nullptr)
    {
        fullName = getCreatedBy()->getAccount()->getName() + ' ' + getCreatedBy()->getAccount()->getSurname();
        if (fullName.length() > 26) std::cout << std::setw(26) << fullName.substr(0, 23) + "..." << '|';
        else std::cout << std::setw(26) << fullName << '|';
    }
    else std::cout << std::setw(28) << ' ' << '|';
    if (getAssignedTo() != nullptr)
    {
        fullName = getAssignedTo()->getAccount()->getName() + ' ' + getAssignedTo()->getAccount()->getSurname();
        if (fullName.length() > 27) std::cout << std::setw(27) << fullName.substr(0, 24) + "..." << '|';
        else std::cout << std::setw(27) << fullName << '|';
    }
    else std::cout << std::setw(27) << ' ' << '|';
}

void Issue::showInfo(std::string status)
{
    std::cout << "Name: " << issueData.name << std::endl;
    std::cout << "Description: " << issueData.description << std::endl;
    std::cout << "Creation date: " << issueData.dateCreated << std::endl;
    std::cout << "Resolve date: " << issueData.dateResolved << std::endl;
    if (getCreatedBy() == nullptr) std::cout << "Created by: N/A" << std::endl;
    else
        std::cout << "Created by: " << getCreatedBy()->getAccount()->getName() + ' ' + getCreatedBy()->
            getAccount()->getSurname() << std::endl;
    if (getAssignedTo() == nullptr) std::cout << "Responsible: Not assigned" << std::endl;
    else
        std::cout << "Responsible: " << getCreatedBy()->getAccount()->getName() + ' ' + getAssignedTo()->
            getAccount()->getSurname() << std::endl;
    std::cout << "Issue status: " << status << std::endl;
    SetConsoleTextAttribute(handle, WHITE << 4 | BLACK);
    std::cout << std::endl << std::endl << " Press any key to return to the project menu..." << std::endl;
    _getch();
}

Issue::Issue(std::string name, std::string description, int priority)
{
    issueData.ID = lastID + 1;
    setName(name);
    setDescription(description);
    setPriority(priority);
    setDateResolved("--.--.----");
    setDateCreatedAuto();
    issuesCount++;
}

Issue::Issue(const Issue& issue)
{
    issueData.ID = issue.issueData.ID;
    issueData.priority = issue.issueData.priority;
    for (short i = 0; i < 81; i++) issueData.name[i] = issue.issueData.name[i];
    for (short i = 0; i < 2001; i++) issueData.description[i] = issue.issueData.description[i];
    for (short i = 0; i < 11; i++)
    {
        issueData.dateCreated[i] = issue.issueData.dateCreated[i];
        issueData.dateResolved[i] = issue.issueData.dateResolved[i];
    }
    createdBy = issue.createdBy;
    assignedTo = issue.assignedTo;
    issuesCount++;
}

unsigned Issue::issuesCount = 0;
unsigned Issue::lastID = 0;

void Issue::setName(const std::string& name)
{
    if (name.length() > 80) throw std::length_error("Name length cannot exceed 80 characters");
    if (name.length() < 4) throw std::length_error("Name must be at least 4 characters long");
    name.copy(issueData.name, 80);
    issueData.name[name.length()] = '\0';
}

void Issue::setDescription(const std::string& description)
{
    if (description.length() > 2000)
        throw std::length_error(
            "The size of the description cannot exceed 2000 characters");
    description.copy(issueData.description, 2000);
    issueData.description[description.length()] = '\0';
}

void Issue::setDateCreated(const std::string& dateCreated)
{
    checkDate(dateCreated);
    dateCreated.copy(issueData.dateCreated, 11);
}

void Issue::setDateResolved(const std::string& dateResolved)
{
    if (dateResolved != "--.--.----") checkDate(dateResolved);
    dateResolved.copy(issueData.dateResolved, 11);
    if (dateResolved == "--.--.----") issueData.dateResolved[10] = '\0';
}

void Issue::setPriority(short priority)
{
    if (priority < Low || priority > High) throw std::invalid_argument("Incorrect priority value.");
    issueData.priority = priority;
}

bool Issue::changePriority()
{
    SetConsoleTextAttribute(handle, BLACK << 4 | WHITE);
    system("cls");
    SetConsoleTextAttribute(handle, WHITE << 4 | BLACK);
    std::cout << " Choose issue priority level: " << std::endl;

    short command = 1;
    std::vector<std::string> menu;
    menu.emplace_back("    Low     ");
    menu.emplace_back("   Medium   ");
    menu.emplace_back("    High    ");
    if (!printMenu(menu, command, 1)) return false;

    switch (command)
    {
    case 0: setPriority(Low);
        break;
    case 1: setPriority(Medium);
        break;
    case 2: setPriority(High);
        break;
    }
    setConsoleCursorPositionAndColor(0, 4, BLACK << 4 | GREEN);
    return true;
}

bool Issue::changeName()
{
    SetConsoleTextAttribute(handle, BLACK << 4 | WHITE);
    system("cls");

    // Entering issue name
    std::cout << " Current issue name: " << issueData.name << std::endl;
    SetConsoleTextAttribute(handle, WHITE << 4 | BLACK);
    std::cout << " New issue name: ";
    std::string oldName = issueData.name;
    if (!inputField([this](const std::string& str) { setName(str); }, 80, 17, 1, 2))
        return false;

    SetConsoleTextAttribute(handle, BLACK << 4 | WHITE);
    system("cls");
    std::cout << " Old issue name: " << oldName << std::endl;
    std::cout << " New issue name: " << issueData.name << std::endl;
    SetConsoleTextAttribute(handle, BLACK << 4 | GREEN);
    std::cout << " Issue name was successfully changed." << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    return true;
}

bool Issue::changeDescription()
{
    SetConsoleTextAttribute(handle, BLACK << 4 | WHITE);
    system("cls");

    // Entering issue description
    std::string oldDescription = issueData.description;
    std::cout << " Old issue description: " << std::endl << oldDescription << std::endl;
    std::cout << " New issue description: " << std::endl;
    if (!inputField([this](const std::string& str) { setDescription(str); }, 2000, 0, coord.Y, -1, true))
        return false;

    SetConsoleTextAttribute(handle, BLACK << 4 | WHITE);
    system("cls");
    std::cout << " Old issue description: " << std::endl << oldDescription << std::endl;
    std::cout << " New issue description: " << std::endl << issueData.description << std::endl;
    SetConsoleTextAttribute(handle, BLACK << 4 | GREEN);
    std::cout << " Issue description was successfully changed." << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    return true;
}
