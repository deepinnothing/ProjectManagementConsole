#include "Bug.h"
#include <iostream>
#include <filesystem>
#include <thread>
#include <windows.h>

#include "ConsoleEssentials.h"
using namespace ace;

void Bug::setStepsToReproduce(const std::string& stepsToReproduce)
{
    stepsToReproduce.copy(this->stepsToReproduce, 2000);
    this->stepsToReproduce[stepsToReproduce.length()] = '\0';
}

inline bool Bug::inputStepsToReproduce()
{
    return inputField([this](const std::string& str) { setStepsToReproduce(str); }, 2000, 0, coord.Y, -1, true);
}

bool Bug::changeStepsToReproduce()
{
    SetConsoleTextAttribute(handle, BLACK << 4 | WHITE);
    system("cls");

    // Entering steps to reproduce the bug
    std::string oldStepsToReproduce = stepsToReproduce;
    std::cout << " Old steps to reproduce the bug: " << std::endl << oldStepsToReproduce << std::endl;
    std::cout << " New steps to reproduce the bug: " << std::endl;
    if (!inputField([this](const std::string& str) { setStepsToReproduce(str); }, 2000, 0, coord.Y, -1, true))
        return false;

    SetConsoleTextAttribute(handle, BLACK << 4 | WHITE);
    system("cls");
    std::cout << " Old steps to reproduce the bug: " << std::endl << oldStepsToReproduce << std::endl;
    std::cout << " New steps to reproduce the bug: " << std::endl << stepsToReproduce << std::endl;
    SetConsoleTextAttribute(handle, BLACK << 4 | GREEN);
    std::cout << " Steps to reproduce the bug were successfully changed." << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    return true;
}

bool Bug::create()
{
    SetConsoleTextAttribute(handle, BLACK << 4 | WHITE);
    system("cls");

    // Preparation for entering a bug name
    SetConsoleTextAttribute(handle, WHITE << 4 | BLACK);
    std::cout << " Bug name: " << std::endl;

    // Entering the issue name
    if (!inputName()) return false;

    // Preparation for entering a bug description
    setConsoleCursorPositionAndColor(0, 0, BLACK << 4 | WHITE);
    std::cout << " Bug name: " << std::endl << issueData.name;
    SetConsoleTextAttribute(handle, WHITE << 4 | BLACK);
    std::cout << std::endl << " Bug description: " << std::endl;

    // Entering the description
    if (!inputDescription()) return false;

    // Preparation for entering steps to reproduce the bug
    setConsoleCursorPositionAndColor(0, 0, BLACK << 4 | WHITE);
    std::cout << " Bug name: " << std::endl << issueData.name;
    std::cout << std::endl << " Bug description:\n" << issueData.description << std::endl;
    setConsoleCursorPositionAndColor(0, coord.Y, WHITE << 4 | BLACK);
    std::cout << " Steps to reproduce the bug:" << std::endl;

    // Entering the steps to reproduce the bug
    if (!inputStepsToReproduce()) return false;

    // Choosing a priority
    changePriority();
    setDateCreatedAuto();
    setCreatedBy(ProjectMember::currentProjectMember);
    issueData.ID = ++lastID;

    return true;
}

bool Bug::change()
{
    SetConsoleTextAttribute(handle, BLACK << 4 | WHITE);
    system("cls");

    short command = 0;
    std::vector<std::string> menu;
    menu.emplace_back(" Change bug name ");
    menu.emplace_back(" Change bug description ");
    menu.emplace_back(" Change steps to reproduce the bug ");
    menu.emplace_back(" Change bug priority ");

    if (!printMenu(menu, command)) return false;

    switch (command)
    {
    case 0: if (!changeName()) return false;
        break;
    case 1: if (!changeDescription()) return false;
        break;
    case 2: if (!changeStepsToReproduce()) return false;
        break;
    case 3: if (!changePriority()) return false;
        break;
    }
}

void Bug::showInfo(std::string status)
{
    SetConsoleTextAttribute(handle, BLACK << 4 | WHITE);
    system("cls");
    std::cout << "ID: " << issueData.ID << std::endl;
    std::cout << "Type: Bug" << std::endl;
    Issue::showInfo(status);
}
