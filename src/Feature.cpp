#include "Feature.h"

#include <conio.h>
#include <iomanip>
#include <iostream>
#include <windows.h>

#include "ConsoleEssentials.h"
using namespace ace;

void Feature::addIssue(std::shared_ptr<Issue> issue)
{
    issue->setAssignedTo(getAssignedTo());
    issues.push_back(issue);
    saveFeatureSubtasks();
}

bool Feature::create()
{
    SetConsoleTextAttribute(handle, BLACK << 4 | WHITE);
    system("cls");

    // Preparation for entering a feature name
    SetConsoleTextAttribute(handle, WHITE << 4 | BLACK);
    std::cout << " Feature name: " << std::endl;

    // Entering the feature name
    if (!inputName()) return false;

    // Preparation for entering a feature description
    setConsoleCursorPositionAndColor(0, 0, BLACK << 4 | WHITE);
    std::cout << " Feature name: " << std::endl << issueData.name;
    SetConsoleTextAttribute(handle, WHITE << 4 | BLACK);
    std::cout << std::endl << " Feature description: " << std::endl;

    // Entering the description
    if (!inputDescription()) return false;

    // Choosing a priority
    changePriority();
    setDateCreatedAuto();
    setCreatedBy(ProjectMember::currentProjectMember);
    issueData.ID = ++lastID;

    return true;
}

bool Feature::change()
{
    SetConsoleTextAttribute(handle, BLACK << 4 | WHITE);
    system("cls");

    short command = 0;
    std::vector<std::string> menu;
    menu.emplace_back(" Change feature name ");
    menu.emplace_back(" Change feature description ");
    menu.emplace_back(" Change feature priority ");
    printMenu(menu, command);

    switch (command)
    {
    case 0: if (!changeName()) return false;
        break;
    case 1: if (!changeDescription()) return false;
        break;
    case 2: if (!changePriority()) return false;
        break;
    }
    return true;
}

void Feature::print()
{
    std::cout << std::left << std::setw(6) << issueData.ID << '|' << std::setw(8) << "Feature" << '|';
    print1();
    std::cout << std::setw(8) << issues.size() << '|';
    print2();
}

void Feature::showInfo(std::string status)
{
    SetConsoleTextAttribute(handle, BLACK << 4 | WHITE);
    system("cls");

    std::cout << "ID: " << issueData.ID << std::endl;
    std::cout << "Type: Feature" << std::endl;
    std::cout << "Name: " << issueData.name << std::endl;
    std::cout << "Description: " << issueData.description << std::endl;
    std::cout << "Number of subtasks: " << issues.size() << std::endl;
    std::cout << "Creation date: " << issueData.dateCreated << std::endl;
    std::cout << "Resolve date: " << issueData.dateResolved << std::endl;
    if (getCreatedBy() == nullptr) std::cout << "Created by: N/A" << std::endl;
    else
        std::cout << "Created by: " << getCreatedBy()->getAccount()->getName() + ' ' + getCreatedBy()->
            getAccount()->getSurname() << std::endl;
    if (getAssignedTo() == nullptr) std::cout << "Responsible: Not assigned" << std::endl;
    else
        std::cout << "Responsible: " << getCreatedBy()->getAccount()->getName() + ' ' +
            getAssignedTo()->getAccount()->getSurname() << std::endl;
    std::cout << "Feature status: " << status << std::endl;
    SetConsoleTextAttribute(handle, WHITE << 4 | BLACK);
    std::cout << std::endl << std::endl << " Press any key to return to the project menu..." << std::endl;
    _getch();
}
