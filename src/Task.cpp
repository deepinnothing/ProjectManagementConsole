#include "Task.h"
#include "ConsoleEssentials.h"
using namespace ace;

#include <iostream>
#include <vector>
#include <windows.h>

bool Task::create()
{
    SetConsoleTextAttribute(handle, BLACK << 4 | WHITE);
    system("cls");

    // Preparation for entering a task name
    SetConsoleTextAttribute(handle, WHITE << 4 | BLACK);
    std::cout << " Task name: " << std::endl;

    // Entering the task name
    if (!inputName()) return false;

    // Preparation for entering a task description
    setConsoleCursorPositionAndColor(0, 0, BLACK << 4 | WHITE);
    std::cout << " Task name: " << std::endl << issueData.name;
    SetConsoleTextAttribute(handle, WHITE << 4 | BLACK);
    std::cout << std::endl << " Task description: " << std::endl;

    // Entering the description
    if (!inputDescription()) return false;

    // Choosing a task priority level
    changePriority();
    setDateCreatedAuto();
    setCreatedBy(ProjectMember::currentProjectMember);
    issueData.ID = ++lastID;

    return true;
}

bool Task::change()
{
    SetConsoleTextAttribute(handle, BLACK << 4 | WHITE);
    system("cls");

    short command = 0;
    std::vector<std::string> menu;
    menu.emplace_back(" Change task name ");
    menu.emplace_back(" Change task description ");
    menu.emplace_back(" Change task priority ");
    if (!printMenu(menu, command)) return false;

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

void Task::showInfo(std::string status)
{
    SetConsoleTextAttribute(handle, BLACK << 4 | WHITE);
    system("cls");
    std::cout << "ID: " << issueData.ID << std::endl;
    std::cout << "Type: Task" << std::endl;
    Issue::showInfo(status);
}
