#pragma once

#include <iomanip>

#include "Issue.h"

// Technical task
class Task : public Issue
{
public:
    short getType() { return TASK; }
    bool create();
    bool change();

    void print()
    {
        std::cout << std::left << std::setw(6) << issueData.ID << '|' << std::setw(8) << "Task" << '|';
        print1();
        std::cout << std::setw(8) << ' ' << '|';
        print2();
    }

    void showInfo(std::string);

    Task(std::string name = "Task name", std::string description = "Task description\nMaximum 2000 characters",
         int priority = Medium) :
        Issue(name, description, priority)
    {
    }

    Task(const Task& task) : Issue(task)
    {
    }
};
