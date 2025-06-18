#include "Feature.h"
#include "Task.h"
#include "Bug.h"
#include "Project.h"

#include <algorithm>
#include <fstream>

// Functions for saving and loading subtasks
void Feature::saveFeatureSubtasks()
{
    std::ofstream fout(
        std::to_string(Project::currentProject->getID()) + "\\Feature" + std::to_string(issueData.ID) + "_Subtasks",
        std::ios::binary);
    for (short i = 0; i < issues.size(); i++)
    {
        short type = issues[i]->getType();
        unsigned ID;
        fout.write(reinterpret_cast<char*>(&type), sizeof(type));
        fout.write(reinterpret_cast<char*>(&issues[i]->issueData), sizeof(issues[i]->issueData));
        if (type == BUG)
        {
            std::shared_ptr<Bug> writeBug = std::static_pointer_cast<Bug>(issues[i]);
            fout.write(writeBug->stepsToReproduce, sizeof(writeBug->stepsToReproduce));
        }
        if (issues[i]->createdBy == nullptr) ID = 0;
        else ID = issues[i]->createdBy->getAccount()->getID();
        fout.write(reinterpret_cast<char*>(&ID), sizeof(ID));
        if (issues[i]->assignedTo == nullptr) ID = 0;
        else ID = issues[i]->assignedTo->getAccount()->getID();
        fout.write(reinterpret_cast<char*>(&ID), sizeof(ID));
    }
}

void Feature::loadFeatureSubtasks()
{
    std::ifstream fin(
        std::to_string(Project::currentProject->ID) + "\\Feature" + std::to_string(issueData.ID) + "_Subtasks",
        std::ios::binary);
    short type;
    unsigned readID;
    while (fin.read(reinterpret_cast<char*>(&type), sizeof(type)))
    {
        std::shared_ptr<Issue> readIssue;
        if (type == TASK) readIssue = std::make_shared<Task>();
        if (type == BUG) readIssue = std::make_shared<Bug>();
        fin.read(reinterpret_cast<char*>(&readIssue->issueData), sizeof(readIssue->issueData));
        lastID = std::max(readIssue->issueData.ID, lastID);
        if (type == BUG)
        {
            std::shared_ptr<Bug> readBug = std::static_pointer_cast<Bug>(readIssue);
            fin.read(readBug->stepsToReproduce, sizeof(readBug->stepsToReproduce));
        }
        fin.read(reinterpret_cast<char*>(&readID), sizeof(readID));
        if (readID != 0)
            for (short i = 0; i < Project::currentProject->projectMembers.size(); i++)
                if (Project::currentProject->projectMembers[i]->getAccount()->getID() == readID)
                {
                    readIssue->setCreatedBy(Project::currentProject->projectMembers[i]);
                    break;
                }
        fin.read(reinterpret_cast<char*>(&readID), sizeof(readID));
        if (readID != 0)
            for (short i = 0; i < Project::currentProject->projectMembers.size(); i++)
                if (Project::currentProject->projectMembers[i]->getAccount()->getID() == readID)
                {
                    readIssue->setAssignedTo(Project::currentProject->projectMembers[i]);
                    break;
                }
        issues.push_back(readIssue);
    }
}
