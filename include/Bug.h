#pragma once
#include <iomanip>
#include <string>

#include "Issue.h"

class Bug : public Issue
{
    friend Project;
    friend Feature;
    char stepsToReproduce[2001];

public:
    std::string getStepsToReproduce() const { return stepsToReproduce; }
    void setStepsToReproduce(const std::string& stepsToReproduce);
    short getType() { return BUG; }
    inline bool inputStepsToReproduce();
    bool changeStepsToReproduce();
    bool create();
    bool change();

    void print()
    {
        std::cout << std::left << std::setw(6) << issueData.ID << '|' << std::setw(8) << "Bug" << '|';
        print1();
        std::cout << std::setw(8) << ' ' << '|';
        print2();
    }

    void showInfo(std::string);

    Bug(std::string name = "Bug name", std::string description = "Bug description\nMaximum 2000 characters",
        std::string stepsToReproduce = "Steps to reproduce the bug\nMaximum 2000 characters", int priority = Medium) :
        Issue(name, description, priority)
    {
        setStepsToReproduce(stepsToReproduce);
    }

    Bug(const Bug& bug) : Issue(bug)
    {
        for (int i = 0; i < 2001; i++)
            this->stepsToReproduce[i] = bug.stepsToReproduce[i];
    }
};
