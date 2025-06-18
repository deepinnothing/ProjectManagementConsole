#include "Project.h"
#include "Feature.h"
#include "Task.h"
#include "Bug.h"
#include "Pair.h"
#include "ConsoleEssentials.h"
using namespace ace;

#include <algorithm>
#include <fstream>
#include <iostream>
#include <windows.h>
#include <conio.h>
#include <filesystem>
#include <thread>

class Task;

void Project::setDateCreated(std::string dateCreated)
{
    checkDate(dateCreated);
    dateCreated.copy(this->dateCreated, 11);
}

void Project::loadProject()
{
    loadList();
    loadList(toDoList, std::to_string(ID) + "\\ToDoList");
    loadList(inProgress, std::to_string(ID) + "\\InProgress");
    loadList(inReview, std::to_string(ID) + "\\InReview");
    loadList(done, std::to_string(ID) + "\\Done");
}

void Project::saveAllLists()
{
    saveList();
    saveList(toDoList, std::to_string(ID) + "\\ToDoList");
    saveList(inProgress, std::to_string(ID) + "\\InProgress");
    saveList(inReview, std::to_string(ID) + "\\InReview");
    saveList(done, std::to_string(ID) + "\\Done");
}

Project::Project(std::string name, std::string description)
{
    ID = 0;
    name.copy(this->name, 80);
    this->name[name.length()] = '\0';
    description.copy(this->description, 2000);
    this->description[description.length()] = '\0';
    // Set the current date as creation date
    std::string dateCreated;
    time_t now = time(0);
    tm* ltm = new tm;
    localtime_s(ltm, &now);
    if (ltm->tm_mday < 10) dateCreated = '0';
    dateCreated += std::to_string(ltm->tm_mday) + '.';
    if (ltm->tm_mon < 10) dateCreated += '0';
    dateCreated += std::to_string(ltm->tm_mon + 1) + '.' + std::to_string(ltm->tm_year + 1900) + '\0';
    dateCreated.copy(this->dateCreated, 11);
}

Project::Project(const Project& proj)
{
    ID = proj.ID;
    for (short i = 0; i < 81; i++) this->name[i] = proj.name[i];
    for (short i = 0; i < 2001; i++) this->description[i] = proj.description[i];
    for (short i = 0; i < 11; i++) this->dateCreated[i] = proj.dateCreated[i];
    this->toDoList = proj.toDoList;
    this->inProgress = proj.inProgress;
    this->inReview = proj.inReview;
    this->done = proj.done;
    this->projectMembers = proj.projectMembers;
}

std::shared_ptr<Project> Project::currentProject = std::make_shared<Project>();

void Project::saveList()
{
    std::ofstream fout(std::to_string(ID) + "\\ProjectMembers", std::ios::binary);
    for (short i = 0; i < projectMembers.size(); i++)
    {
        fout.write(reinterpret_cast<char*>(&projectMembers[i]->account->ID), sizeof(unsigned));
        fout.write(projectMembers[i]->role, sizeof(projectMembers[i]->role));
    }
}

void Project::saveList(std::vector<std::shared_ptr<Issue>>& list, const std::string& path)
{
    std::ofstream fout(path, std::ios::binary);
    for (short i = 0; i < list.size(); i++)
    {
        unsigned ID;
        short type = list[i]->getType();
        fout.write(reinterpret_cast<char*>(&type), sizeof(type));

        fout.write(reinterpret_cast<char*>(&list[i]->issueData), sizeof(list[i]->issueData));
        if (type == BUG)
        {
            std::shared_ptr<Bug> writeBug = std::static_pointer_cast<Bug>(list[i]);
            fout.write(writeBug->stepsToReproduce, sizeof(writeBug->stepsToReproduce));
        }
        if (list[i]->createdBy == nullptr) ID = 0;
        else ID = list[i]->createdBy->getAccount()->getID();
        fout.write(reinterpret_cast<char*>(&ID), sizeof(ID));
        if (list[i]->assignedTo == nullptr) ID = 0;
        else ID = list[i]->assignedTo->getAccount()->getID();
        fout.write(reinterpret_cast<char*>(&ID), sizeof(ID));
        if (type == FEATURE) std::static_pointer_cast<Feature>(list[i])->saveFeatureSubtasks();
    }
}

void Project::setName(std::string name)
{
    if (name.length() > 80) throw std::length_error("Name length cannot exceed 80 characters");
    if (name.length() < 4) throw std::length_error("Name must be at least 4 characters long");
    name.copy(this->name, 80);
    this->name[name.length()] = '\0';
}

void Project::setDescription(std::string description)
{
    if (description.length() > 2000) throw std::length_error("Description length cannot exceed 2000 characters");
    description.copy(this->description, 2000);
    this->description[description.length()] = '\0';
}

void Project::loadList()
{
    std::ifstream fin(std::to_string(ID) + "\\ProjectMembers", std::ios::binary);
    unsigned readID;
    while (fin.read(reinterpret_cast<char*>(&readID), sizeof(readID)))
    {
        std::shared_ptr<ProjectMember> readProjectMember = std::make_shared<ProjectMember>();
        fin.read(readProjectMember->role, sizeof(readProjectMember->role));
        if (readID == Account::currentAccount->ID) projectMembers.push_back(ProjectMember::currentProjectMember);
        else
        {
            readProjectMember->account = std::make_shared<Account>();
            std::ifstream accs("Accounts", std::ios::binary);
            while (accs.read(reinterpret_cast<char*>(&readProjectMember->account->ID), sizeof(unsigned)))
            {
                accs.read(readProjectMember->account->login, sizeof(readProjectMember->account->login));
                accs.read(readProjectMember->account->password, sizeof(readProjectMember->account->password));
                accs.read(readProjectMember->account->name, sizeof(readProjectMember->account->name));
                accs.read(readProjectMember->account->surname, sizeof(readProjectMember->account->surname));
                if (readID == readProjectMember->account->ID)
                {
                    if (readProjectMember->account->ID == Account::currentAccount->ID)
                        readProjectMember->account = Account::currentAccount;
                    break;
                }
            }
            projectMembers.push_back(readProjectMember);
        }
    }
}

void Project::loadList(std::vector<std::shared_ptr<Issue>>& list, const std::string& path)
{
    std::ifstream fin(path, std::ios::binary);
    short type;
    unsigned readID;
    while (fin.read(reinterpret_cast<char*>(&type), sizeof(type)))
    {
        std::shared_ptr<Issue> readIssue;
        if (type == FEATURE) readIssue = std::make_shared<Feature>();
        if (type == TASK) readIssue = std::make_shared<Task>();
        if (type == BUG) readIssue = std::make_shared<Bug>();
        fin.read(reinterpret_cast<char*>(&readIssue->issueData), sizeof(readIssue->issueData));
        Issue::lastID = std::max(readIssue->issueData.ID, Issue::lastID);
        if (type == BUG)
        {
            std::shared_ptr<Bug> readBug = std::static_pointer_cast<Bug>(readIssue);
            fin.read(readBug->stepsToReproduce, sizeof(readBug->stepsToReproduce));
        }
        fin.read(reinterpret_cast<char*>(&readID), sizeof(readID));
        if (readID != 0)
            for (short i = 0; i < projectMembers.size(); i++)
                if (projectMembers[i]->account->ID == readID)
                {
                    readIssue->setCreatedBy(projectMembers[i]);
                    break;
                }
        fin.read(reinterpret_cast<char*>(&readID), sizeof(readID));
        if (readID != 0)
            for (short i = 0; i < projectMembers.size(); i++)
                if (projectMembers[i]->account->ID == readID)
                {
                    readIssue->setAssignedTo(projectMembers[i]);
                    break;
                }
        if (type == FEATURE)
        {
            std::shared_ptr<Feature> readFeature = std::static_pointer_cast<Feature>(readIssue);
            readFeature->loadFeatureSubtasks();
        }
        list.push_back(readIssue);
    }
}

void Project::rewriteToFile()
{
    std::fstream f("Projects", std::ios::binary | std::ios::in | std::ios::out);
    Project readProject;
    while (f.read(reinterpret_cast<char*>(&readProject.ID), sizeof(ID)))
    {
        if (readProject.ID == ID) break;
        f.read(readProject.name, sizeof(name));
        f.read(readProject.description, sizeof(description));
        f.read(readProject.dateCreated, sizeof(dateCreated));
    }
    f.seekp(f.tellg());
    f.write(name, sizeof(name));
    f.write(description, sizeof(description));
}

void Project::create()
{
    SetConsoleTextAttribute(handle, BLACK << 4 | WHITE);
    system("cls");

    // Entering project name
    SetConsoleTextAttribute(handle, WHITE << 4 | BLACK);
    std::cout << " Project name: " << std::endl;
    inputField([this](const std::string& str) { setName(str); },
               80, 0, 1, 2);

    // Entering project description
    setConsoleCursorPositionAndColor(0, 0, BLACK << 4 | WHITE);
    std::cout << " Project name: " << std::endl << name << std::endl;
    SetConsoleTextAttribute(handle, WHITE << 4 | BLACK);
    std::cout << " Project description (press Ctrl+J for a new line): " << std::endl;
    updateConsoleCursorPosition();
    inputField([this](const std::string& str) { setDescription(str); },
               2000, 0, coord.Y, -1, true);

    setConsoleCursorPositionAndColor(0, 0, BLACK << 4 | WHITE);
    std::cout << " Project name: " << std::endl << name << std::endl;
    std::cout << " Project description:                                    " << std::endl << description << std::endl;
    setCurrentDateAsDateCreated();
    std::ofstream fout("Projects", std::ios::binary | std::ios::app | std::ios::ate);
    ID = fout.tellp() / (sizeof(ID) + sizeof(name) + sizeof(description) + sizeof(dateCreated));
    if (fout.tellp() % (sizeof(ID) + sizeof(name) + sizeof(description) + sizeof(dateCreated)) != 0) ID++;
    fout.write(reinterpret_cast<char*>(&ID), sizeof(ID));
    fout.write(name, sizeof(name));
    fout.write(description, sizeof(description));
    fout.write(dateCreated, sizeof(dateCreated));
    std::filesystem::create_directory(".\\" + std::to_string(ID));
    ProjectMember::currentProjectMember->setRole("Administrator");
    ProjectMember::currentProjectMember->setAccount(Account::currentAccount);
    projectMembers.push_back(ProjectMember::currentProjectMember);
    saveList();
    fout.close();
    SetConsoleTextAttribute(handle, BLACK << 4 | GREEN);
    std::cout << " Project created successfully." << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    actions();
}

void Project::actions()
{
    // Display the menu with available actions
    std::vector<Pair<short, std::string>> menu;
    if (ProjectMember::currentProjectMember->getRole() == "Administrator")
    {
        menu.emplace_back(0, " Change project name");
        menu.emplace_back(1, " Change project description");
        menu.emplace_back(24, " Delete project");
        menu.emplace_back(2, " Add new project member");
        menu.emplace_back(22, " Change project member role");
        menu.emplace_back(18, " Display all project members");
        menu.emplace_back(20, " Remove project member");
        menu.emplace_back(19, " Assign an issue to a project member");
    }
    if (ProjectMember::currentProjectMember->getRole() == "Client / Designer" || ProjectMember::currentProjectMember->
        getRole() == "Administrator")
    {
        menu.emplace_back(3, " Generate report and save to file");
        menu.emplace_back(4, " Add new feature");
    }
    if (ProjectMember::currentProjectMember->getRole() == "Developer" || ProjectMember::currentProjectMember->getRole()
        == "Administrator")
    {
        menu.emplace_back(7, " Add new task");
        menu.emplace_back(13, " Assign an issue to self");
        menu.emplace_back(9, " Submit an issue solution for review");
    }
    if (ProjectMember::currentProjectMember->getRole() == "Tester" || ProjectMember::currentProjectMember->getRole() ==
        "Administrator")
    {
        menu.emplace_back(10, " Report a bug");
        menu.emplace_back(11, " Confirm an issue solution");
    }
    menu.emplace_back(21, " Display project information");
    menu.emplace_back(12, " Display issues");
    menu.emplace_back(14, " Sort issues");
    menu.emplace_back(8, " Display detailed issue information");
    menu.emplace_back(5, " Edit issue");
    menu.emplace_back(6, " Delete issue");
    menu.emplace_back(23, " Help");
    menu.emplace_back(17, " Close the project");

    while (true)
    {
        SetConsoleTextAttribute(handle, BLACK << 4 | WHITE);
        system("cls");

        short command = 0;
        if (!printMenu(menu, command)) return;
        SetConsoleTextAttribute(handle, BLACK << 4 | WHITE);

        switch (menu[command].getFirst())
        {
        case 0: changeProjectName();
            break;
        case 1: changeProjectDescription();
            break;
        case 2: addProjectMember();
            break;
        case 3: makeReport();
            break;
        case 4: addFeature();
            break;
        case 5: changeIssue();
            break;
        case 6: removeIssue();
            break;
        case 7: addTask();
            break;
        case 8: showIssueInfo();
            break;
        case 9: sendOnReview();
            break;
        case 10: addBug();
            break;
        case 11: confirmSolution();
            break;
        case 12: showList();
            break;
        case 13: selfAssignIssue();
            break;
        case 14: sortIssues();
            break;
        case 17: system("cls");
            return;
        case 18: showAllProjectMembers();
            break;
        case 19: assignIssueToSomePerson();
            break;
        case 20: removeProjectMember();
            break;
        case 21: showProjectInfo();
            break;
        case 22: changeProjectMemberRole();
            break;
        case 23: help();
            break;
        case 24: if (removeProject()) return;
        }
    }
}

void Project::help()
{
    SetConsoleTextAttribute(handle, BLACK << 4 | WHITE);
    system("cls");
    std::cout << "\tThis help describes the main elements you will work with. " << std::endl << std::endl;
    std::cout << "\tThe first thing you encountered when starting the program is accounts and authorization. ";
    std::cout <<
        "Each account stores login and password information used for authorization, and the registered person's name and surname. ";
    std::cout << "All accounts are stored in one place and are completely independent of other system components." <<
        std::endl <<
        std::endl;
    std::cout << "\tNext, you encountered projects. This is the largest system element you will work with. ";
    std::cout << "A project, simply put, is a collection of tasks united by a common goal or context. ";
    std::cout <<
        "Just as one user can participate in multiple projects, one project can be distributed among multiple users. "
        << std::endl << std::endl;
    std::cout << "\tThus, each project has its own team. ";
    std::cout << "Each team (project) member has a specific role that allows them to focus on specific tasks. ";
    std::cout <<
        "For example, a client/designer proposes innovations and contributes ideas for new functionality with high-level descriptions for the project. ";
    std::cout <<
        "A developer (programmer), in turn, implements the client/designer's ideas, sometimes adding technical corrections to existing tasks."
        << std::endl << std::endl;
    std::cout << "\tEach project contains several task lists:" << std::endl;
    std::cout << "* Upcoming tasks (work on which will start soon);" << std::endl;
    std::cout << "* Active tasks (work on which is currently in progress);" << std::endl;
    std::cout << "* Tasks under review (testers verify the developers' proper execution of assigned tasks);" <<
        std::endl;
    std::cout << "* Completed tasks (tasks implemented by developers and verified by testers);" << std::endl;
    std::cout << "* Canceled tasks (tasks removed from the development process)." << std::endl;
    std::cout << "\tThe main work will involve these lists and the tasks contained within them. ";
    std::cout << "Tasks are of two types:" << std::endl;
    std::cout << "* Feature (contains a high-level description of development goals. Created by the client/designer);"
        <<
        std::endl;
    std::cout <<
        "* Task (a small technical task created by a developer that complements a feature with specific implementation steps);"
        << std::endl;
    std::cout <<
        "* Bug (an additional unplanned task created by a tester in case of incorrect implementation by a developer)."
        << std::endl;
    SetConsoleTextAttribute(handle, WHITE << 4 | BLACK);
    std::cout << "Press any key to return to the project menu...";
    _getch();
}

void Project::showProjectInfo()
{
    SetConsoleTextAttribute(handle, BLACK << 4 | WHITE);
    system("cls");
    std::cout << " Project name: " << name << std::endl << std::endl;
    std::cout << " Project description: " << std::endl << description << std::endl << std::endl;
    std::cout << " Project creation date: " << dateCreated << std::endl << std::endl;
    SetConsoleTextAttribute(handle, WHITE << 4 | BLACK);
    std::cout << "Press any key to return to the project menu...";
    _getch();
}

void Project::changeProjectDescription()
{
    system("cls");

    // Entering project description
    SetConsoleTextAttribute(handle, BLACK << 4 | WHITE);
    std::string oldDescription = description;
    std::cout << " Old project description: " << std::endl << oldDescription << std::endl;
    std::cout << " New project description: " << std::endl;
    updateConsoleCursorPosition();
    inputField([this](const std::string& str) { setDescription(str); },
               2000, 0, coord.Y, -1, true);

    rewriteToFile();
    SetConsoleTextAttribute(handle, BLACK << 4 | WHITE);
    system("cls");
    std::cout << " Old project description: " << std::endl << oldDescription << std::endl;
    std::cout << " New project description: " << std::endl << description << std::endl;
    SetConsoleTextAttribute(handle, BLACK << 4 | GREEN);
    std::cout << " Project description successfully changed." << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
}

void Project::changeProjectName()
{
    system("cls");

    SetConsoleTextAttribute(handle, BLACK << 4 | WHITE);
    std::cout << " Current project name: " << std::endl << name << std::endl;
    SetConsoleTextAttribute(handle, WHITE << 4 | BLACK);
    std::cout << " New project name: " << std::endl;
    std::string oldName = name;
    inputField([this](const std::string& str) { setName(str); },
               80, 0, 3, 4);

    rewriteToFile();
    SetConsoleTextAttribute(handle, BLACK << 4 | WHITE);
    system("cls");
    std::cout << " Old project name: " << std::endl << oldName << std::endl;
    std::cout << " New project name: " << std::endl << name << std::endl;
    SetConsoleTextAttribute(handle, BLACK << 4 | GREEN);
    std::cout << " Project name was successfully changed." << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
}

void Project::showAllProjectMembers()
{
    SetConsoleTextAttribute(handle, BLACK << 4 | WHITE);
    system("cls");
    std::cout << std::left << std::setw(10) << "    ID" << '|' << std::setw(20) << "        Login" << '|' <<
        std::setw(20) << "        Name" << '|' << std::setw(20) << "       Surname" << '|' << std::setw(20) <<
        "   Project role" << std::endl;
    for (short i = 0; i < projectMembers.size(); i++)
        std::cout << *projectMembers[i] << std::endl;
    SetConsoleTextAttribute(handle, WHITE << 4 | BLACK);
    std::cout << "Press any key to return to the project menu..." << std::endl;
    _getch();
}

void Project::removeProjectMember()
{
    if (projectMembers.size() <= 1)
    {
        SetConsoleTextAttribute(handle, BLACK << 4 | RED);
        system("cls");
        std::cout << " No one to remove from the project!" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        return;
    }
    SetConsoleTextAttribute(handle, BLACK << 4 | WHITE);
    system("cls");
    short projectMemberChoice = 0;
    if (!printMenu(projectMembers, projectMemberChoice)) return;
    if (projectMembers[projectMemberChoice]->account->ID == Account::currentAccount->ID)
    {
        setConsoleCursorPositionAndColor(0, projectMembers.size() + 1, BLACK << 4 | RED);
        std::cout << " You cannot remove yourself." << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        return;
    }
    projectMembers.erase(projectMembers.begin() + projectMemberChoice);
    saveList();
    setConsoleCursorPositionAndColor(0, projectMembers.size() + 2, BLACK << 4 | GREEN);
    std::cout << " Member successfully removed from the project.";
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
}

void Project::addProjectMember()
{
    SetConsoleTextAttribute(handle, BLACK << 4 | WHITE);
    system("cls");
    SetConsoleTextAttribute(handle, WHITE << 4 | BLACK);
    std::cout << "Enter the login or ID of the member you want to add to the project:" << std::endl;
    unsigned searchID;
    std::string searchLogin;
    std::cin.unsetf(std::ios::skipws);
    while (true)
    {
        std::cin >> searchID;
        if (std::cin.fail())
        {
            std::cin.clear();
            getline(std::cin, searchLogin, '\n');
            if (searchLogin.length() < 4 || searchLogin.length() > 20)
            {
                std::cout << "Login length must be between 4 and 20 characters. Try again: ";
                continue;
            }
            break;
        }
        if (std::cin.get() != '\n')
        {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid ID input. Try again: ";
            continue;
        }
        searchLogin.clear();
        break;
    }
    for (short i = 0; i < projectMembers.size(); i++)
        if (!searchLogin.empty() && projectMembers[i]->account->login == searchLogin ||
            searchLogin.empty() && projectMembers[i]->account->ID == searchID)
        {
            setConsoleCursorPositionAndColor(0, 2, BLACK << 4 | RED);
            std::cout << " This user is already in the project." << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(2000));
            return;
        }
    std::ifstream fin("Accounts", std::ios::binary);
    std::shared_ptr<Account> checkingAccount = std::make_shared<Account>();
    while (fin.read(reinterpret_cast<char*>(checkingAccount.get()), sizeof(Account)))
        if (!searchLogin.empty() && searchLogin == checkingAccount->login ||
            searchLogin.empty() && searchID == checkingAccount->ID)
        {
            std::shared_ptr<ProjectMember> projectMember = std::make_shared<ProjectMember>();
            projectMember->account = checkingAccount;
            projectMember->changeRole();
            projectMembers.push_back(projectMember);
            saveList();
            setConsoleCursorPositionAndColor(0, 5, BLACK << 4 | GREEN);
            std::cout << " New member successfully added to the project." << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(2000));
            return;
        }
    SetConsoleTextAttribute(handle, BLACK << 4 | RED);
    std::cout << " No user with this login or ID exists." << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
}

void Project::changeProjectMemberRole()
{
    SetConsoleTextAttribute(handle, BLACK << 4 | WHITE);
    system("cls");
    std::vector<std::shared_ptr<ProjectMember>> members;
    for (short i = 0; i < projectMembers.size(); i++)
        if (projectMembers[i] != ProjectMember::currentProjectMember)
            members.push_back(projectMembers[i]);
    if (members.empty())
    {
        SetConsoleTextAttribute(handle, BLACK << 4 | RED);
        system("cls");
        std::cout << " No one to change!" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        return;
    }
    short projectMemberChoice = 0;
    if (!printMenu(members, projectMemberChoice)) return;
    SetConsoleTextAttribute(handle, BLACK << 4 | WHITE);
    members[projectMemberChoice]->changeRole();
    saveList();
    setConsoleCursorPositionAndColor(0, 5, BLACK << 4 | GREEN);
    std::cout << " Member role successfully changed.";
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
}

bool Project::removeProject()
{
    SetConsoleTextAttribute(handle, BLACK << 4 | WHITE);
    system("cls");
    short command = 1;
    std::cout << " Are you sure you want to completely delete the project?" << std::endl;
    std::vector<std::string> menu;
    menu.emplace_back("                       Yes                       ");
    menu.emplace_back("                       No                        ");
    if (!printMenu(menu, command, 1)) return false;
    if (command == 1) return false;

    // Read existing projects
    std::vector<std::shared_ptr<Project>> projects;
    std::ifstream fin("Projects", std::ios::binary);
    unsigned readID;
    char readName[81];
    char readDescription[2001];
    char readDateCreated[11];
    while (fin.read(reinterpret_cast<char*>(&readID), sizeof(readID)))
    {
        fin.read(readName, sizeof(readName));
        fin.read(readDescription, sizeof(readDescription));
        fin.read(readDateCreated, sizeof(readDateCreated));
        std::shared_ptr<Project> readProject = std::make_shared<Project>();
        readProject->setID(readID);
        readProject->setName(readName);
        readProject->setDescription(readDescription);
        readProject->setDateCreated(readDateCreated);
        if (readID != ID) projects.push_back(readProject);
    }
    fin.close();
    std::filesystem::remove_all(std::to_string(ID));
    if (projects.empty()) std::filesystem::remove("Projects");
    else
    {
        std::ofstream fout("Projects", std::ios::binary);
        for (short i = 0; i < projects.size(); i++)
        {
            fout.write(reinterpret_cast<char*>(&projects[i]->ID), sizeof(&projects[i]->ID));
            fout.write(reinterpret_cast<char*>(&projects[i]->name), sizeof(&projects[i]->name));
            fout.write(reinterpret_cast<char*>(&projects[i]->description), sizeof(&projects[i]->description));
            fout.write(reinterpret_cast<char*>(&projects[i]->dateCreated), sizeof(&projects[i]->dateCreated));
        }
    }
    setConsoleCursorPositionAndColor(0, 3, BLACK << 4 | GREEN);
    std::cout << " Project was successfully deleted.";
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    return true;
}

void Project::addFeature()
{
    std::shared_ptr<Feature> newFeature = std::make_shared<Feature>();
    if (!newFeature->create()) return;
    toDoList.push_back(newFeature);
    saveList(toDoList, std::to_string(ID) + "\\ToDoList");
    SetConsoleTextAttribute(handle, BLACK << 4 | GREEN);
    std::cout << std::endl << " Feature successfully added." << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
}

void Project::addTask()
{
    short i, count = 0;
    std::vector<std::shared_ptr<Issue>> features;
    for (i = 0; i < toDoList.size(); i++)
        if (toDoList[i]->getType() == FEATURE)
        {
            features.push_back(toDoList[i]);
            count++;
        }
    for (i = 0; i < inProgress.size(); i++)
        if (inProgress[i]->getType() == FEATURE)
            features.push_back(inProgress[i]);
    SetConsoleTextAttribute(handle, BLACK << 4 | WHITE);
    system("cls");
    short featureChoice = 0;
    if (!features.empty())
        if (!printMenu(features, featureChoice, "Create an independent task", count))
            return;

    std::shared_ptr<Task> newTask = std::make_shared<Task>();
    if (!newTask->create()) return;
    if (featureChoice == features.size())
    {
        toDoList.push_back(newTask);
        saveList(toDoList, std::to_string(ID) + "\\ToDoList");
    }
    else std::static_pointer_cast<Feature>(features[featureChoice])->addIssue(newTask);
    SetConsoleTextAttribute(handle, BLACK << 4 | GREEN);
    std::cout << " Task successfully added.";
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
}

void Project::addBug()
{
    std::vector<std::shared_ptr<Issue>> features;
    for (short i = 0; i < inReview.size(); i++)
        if (inReview[i]->getType() == FEATURE)
            features.push_back(inReview[i]);
    SetConsoleTextAttribute(handle, BLACK << 4 | WHITE);
    system("cls");
    short featureChoice = 0;
    if (!features.empty())
        if (!printMenu(features, featureChoice, "Create an independent bug", -1, -1))
            return;

    std::shared_ptr<Bug> newBug = std::make_shared<Bug>();
    if (!newBug->create()) return;
    if (featureChoice == features.size())
    {
        toDoList.push_back(newBug);
        saveList(toDoList, std::to_string(ID) + "\\ToDoList");
    }
    else
    {
        std::static_pointer_cast<Feature>(features[featureChoice])->addIssue(newBug);
        inProgress.push_back(features[featureChoice]);
        for (short i = 0; i < inReview.size(); i++)
            if (inReview[i].get() == features[featureChoice].get())
            {
                inReview.erase(inReview.begin() + i);
                break;
            }
        saveList(toDoList, std::to_string(ID) + "\\ToDoList");
        saveList(inReview, std::to_string(ID) + "\\InReview");
    }
    SetConsoleTextAttribute(handle, BLACK << 4 | GREEN);
    std::cout << " Bug report successfully created.";
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
}

void Project::makeFilteredList(std::vector<std::shared_ptr<Issue>>& issues,
                               std::vector<std::shared_ptr<Issue>>& filtered, bool* filter,
                               short& count)
{
    for (short i = 0; i < issues.size(); i++)
    {
        if (issues[i]->getType() == FEATURE && filter[4] == true ||
            issues[i]->getType() == TASK && filter[5] == true ||
            issues[i]->getType() == BUG && filter[6] == true)
            if (issues[i]->getPriority() == High && filter[7] == true ||
                issues[i]->getPriority() == Medium && filter[8] == true ||
                issues[i]->getPriority() == Low && filter[9] == true)
            {
                filtered.push_back(issues[i]);
                count++;
            }
        if (issues[i]->getType() == FEATURE)
        {
            std::shared_ptr<Feature> feat = std::static_pointer_cast<Feature>(issues[i]);
            for (short j = 0; j < feat->issues.size(); j++)
                if (feat->issues[j]->getType() == TASK && filter[5] == true ||
                    feat->issues[j]->getType() == BUG && filter[6] == true)
                    if (feat->issues[j]->getPriority() == High && filter[7] == true ||
                        feat->issues[j]->getPriority() == Medium && filter[8] == true ||
                        feat->issues[j]->getPriority() == Low && filter[9] == true)
                    {
                        filtered.push_back(feat->issues[j]);
                        count++;
                    }
        }
    }
}


void Project::showFilteredList(bool* filter)
{
    short count1 = 0, count2 = 0, count3 = 0, count4 = 0;
    std::vector<std::shared_ptr<Issue>> issues;
    if (filter[0] == true)
        makeFilteredList(toDoList, issues, filter, count1);
    if (filter[1] == true)
        makeFilteredList(inProgress, issues, filter, count2);
    if (filter[2] == true)
        makeFilteredList(inReview, issues, filter, count3);
    if (filter[3] == true)
        makeFilteredList(done, issues, filter, count4);
    if (issues.empty())
    {
        SetConsoleTextAttribute(handle, BLACK << 4 | RED);
        system("cls");
        std::cout << " Nothing to show with the specified filters!" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        return;
    }
    SetConsoleTextAttribute(handle, BLACK << 4 | WHITE);
    system("cls");
    printIssueTableHeader();
    printIssuesWithStatuses(issues, count1, count2, count3);
    SetConsoleTextAttribute(handle, WHITE << 4 | BLACK);
    std::cout << std::endl << std::endl << " Press any key to return to the project menu..." << std::endl;
    _getch();
}

void Project::showList()
{
    SetConsoleTextAttribute(handle, BLACK << 4 | WHITE);
    system("cls");

    std::vector<std::string> filters;
    filters.emplace_back(" Display to do issues ");
    filters.emplace_back(" Display issues in progress ");
    filters.emplace_back(" Display issues in review ");
    filters.emplace_back(" Display resolved issues ");
    filters.emplace_back(" Display features ");
    filters.emplace_back(" Display tasks ");
    filters.emplace_back(" Display bugs ");
    filters.emplace_back(" Display high-priority issues ");
    filters.emplace_back(" Display medium-priority issues ");
    filters.emplace_back(" Display low-priority issues ");

    bool* filter = new bool[filters.size()];
    for (short i = 0; i < filters.size(); i++) filter[i] = true;
    std::cout << "Select the desired filters. Use the Space key to toggle filters" << std::endl;
    SetConsoleTextAttribute(handle, WHITE << 4 | BLACK);
    for (short i = 0; i < filters.size(); i++)
        std::cout << filters[i] << std::endl;
    short currentFilter = 0;
    while (true)
    {
        // Set specific filters
        coord.X = 0;
        coord.Y = currentFilter + 1;
        SetConsoleCursorPosition(handle, coord);
        if (filter[currentFilter] == true) SetConsoleTextAttribute(handle, WHITE << 4 | BLACK);
        else SetConsoleTextAttribute(handle, BLACK << 4 | WHITE);
        std::cout << filters[currentFilter];
        coord.X = 0;
        SetConsoleCursorPosition(handle, coord);
        getchChar = _getch();
        if (getchChar == ' ') filter[currentFilter] = !filter[currentFilter];
        if (getchChar == 27) return;
        if (getchChar == '\r') break;
        if (getchChar == -32)
        {
            switch (_getch())
            {
            case 72: if (currentFilter != 0) currentFilter--;
                break;
            case 80: if (currentFilter != filters.size() - 1) currentFilter++;
                break;
            }
        }
    }
    showFilteredList(filter);
}

void Project::makeRoleFilteredList(std::vector<std::shared_ptr<Issue>>& issues,
                                   std::vector<std::shared_ptr<Issue>>& filtered, short& count)
{
    for (short i = 0; i < issues.size(); i++)
    {
        if (ProjectMember::currentProjectMember->getRole() == "Administrator" ||
            issues[i]->getType() == FEATURE && ProjectMember::currentProjectMember->getRole() == "Client / Designer" ||
            issues[i]->getType() == TASK && ProjectMember::currentProjectMember->getRole() == "Developer" ||
            issues[i]->getType() == BUG && ProjectMember::currentProjectMember->getRole() == "Tester")
        {
            filtered.push_back(issues[i]);
            count++;
        }
        if (issues[i]->getType() == FEATURE)
        {
            std::shared_ptr<Feature> feat = std::static_pointer_cast<Feature>(issues[i]);
            for (short j = 0; j < feat->issues.size(); j++)
                if (ProjectMember::currentProjectMember->getRole() == "Administrator" ||
                    feat->issues[j]->getType() == FEATURE && ProjectMember::currentProjectMember->getRole() ==
                    "Client / Designer" ||
                    feat->issues[j]->getType() == TASK && ProjectMember::currentProjectMember->getRole() == "Developer"
                    || feat->issues[j]->getType() == BUG && ProjectMember::currentProjectMember->getRole() == "Tester")
                {
                    filtered.push_back(feat->issues[j]);
                    count++;
                }
        }
    }
}

void Project::changeIssue()
{
    short count1 = 0, count2 = 0, count3 = 0;
    std::vector<std::shared_ptr<Issue>> issues;
    makeRoleFilteredList(toDoList, issues, count1);
    makeRoleFilteredList(inProgress, issues, count2);
    makeRoleFilteredList(inReview, issues, count3);

    SetConsoleTextAttribute(handle, BLACK << 4 | WHITE);
    system("cls");
    short issueChoice = 0;
    if (!printMenu(issues, issueChoice, "", count1, count2)) return;
    issues[issueChoice]->change();
    if (issueChoice >= count1 + count2) saveList(inReview, std::to_string(ID) + "\\InReview");
    else if (issueChoice >= count1) saveList(inProgress, std::to_string(ID) + "\\InProgress");
    else saveList(toDoList, std::to_string(ID) + "\\ToDoList");
    SetConsoleTextAttribute(handle, BLACK << 4 | GREEN);
    std::cout << " Issue successfully changed.";
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
}

void Project::removeIssue(std::vector<std::shared_ptr<Issue>>& removeFromList, unsigned removeID,
                          std::string listFileName)
{
    for (short i = 0; i < removeFromList.size(); i++)
    {
        if (removeFromList[i]->issueData.ID == removeID)
        {
            if (removeFromList[i]->getType() == FEATURE)
                std::filesystem::remove(std::to_string(ID) + "\\Feature" + std::to_string(removeID) + "_Subtasks");
            removeFromList.erase(removeFromList.begin() + i);
            saveList(removeFromList, std::to_string(ID) + "\\" + listFileName);
            return;
        }
        if (removeFromList[i]->getType() == FEATURE)
        {
            std::shared_ptr<Feature> feat = std::static_pointer_cast<Feature>(removeFromList[i]);
            for (short j = 0; j < feat->issues.size(); j++)
                if (feat->issues[j]->issueData.ID == removeID)
                {
                    feat->issues.erase(feat->issues.begin() + j);
                    feat->saveFeatureSubtasks();
                    return;
                }
        }
    }
}

void Project::removeIssue()
{
    short count1 = 0, count2 = 0, count3 = 0, count4 = 0;
    std::vector<std::shared_ptr<Issue>> issues;
    makeRoleFilteredList(toDoList, issues, count1);
    makeRoleFilteredList(inProgress, issues, count2);
    makeRoleFilteredList(inReview, issues, count3);
    makeRoleFilteredList(done, issues, count4);

    SetConsoleTextAttribute(handle, BLACK << 4 | WHITE);
    system("cls");
    short issueChoice = 0;
    if (!printMenu(issues, issueChoice, "", count1, count2, count3)) return;
    removeIssue(toDoList, issues[issueChoice]->issueData.ID, "ToDoList");
    removeIssue(inProgress, issues[issueChoice]->issueData.ID, "InProgress");
    removeIssue(inReview, issues[issueChoice]->issueData.ID, "InReview");
    removeIssue(done, issues[issueChoice]->issueData.ID, "Done");
    setConsoleCursorPositionAndColor(0, issues.size() + 1, BLACK << 4 | GREEN);
    std::cout << " Issue removed successfully.";
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
}

void Project::makeExpandedList(std::vector<std::shared_ptr<Issue>>& from, std::vector<std::shared_ptr<Issue>>& to,
                               short& count)
{
    for (short i = 0; i < from.size(); i++)
    {
        to.push_back(from[i]);
        if (from[i]->getType() == FEATURE)
        {
            std::shared_ptr<Feature> feat = std::static_pointer_cast<Feature>(from[i]);
            for (short j = 0; j < feat->issues.size(); j++)
                to.push_back(feat->issues[j]);
            count += feat->issues.size();
        }
    }
    count += from.size();
}


void Project::showIssueInfo()
{
    short count1 = 0, count2 = 0, count3 = 0, count4 = 0;
    std::vector<std::shared_ptr<Issue>> issues;

    makeExpandedList(toDoList, issues, count1);
    makeExpandedList(inProgress, issues, count2);
    makeExpandedList(inReview, issues, count3);
    makeExpandedList(done, issues, count4);

    if (issues.empty())
    {
        SetConsoleTextAttribute(handle, BLACK << 4 | RED);
        system("cls");
        std::cout << " Nothing to show!" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        return;
    }
    SetConsoleTextAttribute(handle, BLACK << 4 | WHITE);
    system("cls");
    short featureChoice = 0;
    if (!printMenu(issues, featureChoice, "", count1, count2, count3)) return;
    std::string status;
    if (featureChoice >= count1 + count2 + count3) status = "Resolved";
    else if (featureChoice >= count1 + count2) status = "In review";
    else if (featureChoice >= count1) status = "In progress";
    else status = "To do";
    issues[featureChoice]->showInfo(status);
}

void Project::assignIssueToSomePerson(bool bSelfAssign)
{
    short count1 = 0, count2 = 0, count3 = 0;
    std::vector<std::shared_ptr<Issue>> issues;

    for (short i = 0; i < toDoList.size(); i++)
    {
        issues.push_back(toDoList[i]);
        count1++;
    }
    makeExpandedList(inProgress, issues, count2);
    makeExpandedList(inReview, issues, count3);

    if (issues.empty())
    {
        SetConsoleTextAttribute(handle, BLACK << 4 | RED);
        system("cls");
        std::cout << " Nothing to assign!" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        return;
    }

    SetConsoleTextAttribute(handle, BLACK << 4 | WHITE);
    system("cls");

    short issueChoice = 0;
    if (!printMenu(issues, issueChoice, "", count1, count2)) return;

    std::shared_ptr<ProjectMember> member;
    if (!bSelfAssign)
    {
        std::vector<std::shared_ptr<ProjectMember>> members;
        SetConsoleTextAttribute(handle, BLACK << 4 | WHITE);
        system("cls");

        for (short i = 0; i < projectMembers.size(); i++)
            if (projectMembers[i]->getRole() == "Administrator" || projectMembers[i]->getRole() ==
                "Developer")
                members.push_back(projectMembers[i]);

        if (members.empty())
        {
            SetConsoleTextAttribute(handle, BLACK << 4 | RED);
            system("cls");
            std::cout << " No one to assign to!" << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(2000));
            return;
        }
        short projectMemberChoice = 0;
        if (!printMenu(members, projectMemberChoice)) return;
        member = members[projectMemberChoice];
        setConsoleCursorPositionAndColor(0, members.size() + 1, BLACK << 4 | GREEN);
    }
    else
    {
        member = ProjectMember::currentProjectMember;
        setConsoleCursorPositionAndColor(0, issues.size() + 1, BLACK << 4 | GREEN);
    }

    issues[issueChoice]->setAssignedTo(member);
    if (issues[issueChoice]->getType() == FEATURE)
    {
        std::shared_ptr<Feature> feat = std::static_pointer_cast<Feature>(issues[issueChoice]);
        for (short i = 0; i < feat->issues.size(); i++)
            feat->issues[i]->setAssignedTo(member);
    }
    for (short i = 0; i < toDoList.size(); i++)
        if (toDoList[i] == issues[issueChoice])
        {
            inProgress.push_back(toDoList[i]);
            toDoList.erase(toDoList.begin() + i);
            break;
        }

    saveList(toDoList, std::to_string(ID) + "\\ToDoList");
    saveList(inProgress, std::to_string(ID) + "\\InProgress");
    saveList(inReview, std::to_string(ID) + "\\InReview");

    std::cout << " Issue with ID " << issues[issueChoice]->issueData.ID << " successfully assigned " <<
        member->getAccount()->getName() + ' ' + member->getAccount()->getSurname() + '.';
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
}

void Project::sendOnReview()
{
    std::vector<std::shared_ptr<Issue>> issues;
    short count1 = 0, count2 = 0;
    for (short i = 0; i < inProgress.size(); i++)
    {
        if (inProgress[i]->getAssignedTo() == ProjectMember::currentProjectMember)
        {
            issues.push_back(inProgress[i]);
            count2++;
        }
    }

    if (issues.empty())
    {
        SetConsoleTextAttribute(handle, BLACK << 4 | RED);
        system("cls");
        std::cout << " You don't have an issue you can submit for review!!" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        return;
    }
    SetConsoleTextAttribute(handle, BLACK << 4 | WHITE);
    system("cls");
    short issueChoice = 0;
    if (!printMenu(issues, issueChoice, "", count1, count2)) return;
    for (short i = 0; i < inProgress.size(); i++)
        if (inProgress[i] == issues[issueChoice])
        {
            inReview.push_back(inProgress[i]);
            inProgress.erase(inProgress.begin() + i);
            break;
        }
    saveList(inProgress, std::to_string(ID) + "\\InProgress");
    saveList(inReview, std::to_string(ID) + "\\InReview");
    coord.X = 0;
    coord.Y = issues.size() + 1;
    SetConsoleCursorPosition(handle, coord);
    std::cout << " Issue with ID " << issues[issueChoice]->issueData.ID <<
        " successfully submitted for review.";
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
}

void Project::confirmSolution()
{
    if (inReview.empty())
    {
        SetConsoleTextAttribute(handle, BLACK << 4 | RED);
        system("cls");
        std::cout << " There is no issues currently in review!" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        return;
    }
    SetConsoleTextAttribute(handle, (BLACK << 4) | WHITE);
    system("cls");
    short issueChoice = 0;
    if (!printMenu(inReview, issueChoice, "", 0, 0, inReview.size())) return;
    inReview[issueChoice]->setDateResolvedAuto();
    done.push_back(inReview[issueChoice]);
    inReview.erase(inReview.begin() + issueChoice);
    saveList(inReview, std::to_string(ID) + "\\InReview");
    saveList(done, std::to_string(ID) + "\\Done");
    setConsoleCursorPositionAndColor(0, inReview.size() + 2, BLACK << 4 | GREEN);
    std::cout << " The resolution of the issue successfully confirmed.";
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
}

void Project::makeReport()
{
    system("cls");
    SetConsoleTextAttribute(handle, BLACK << 4 | WHITE);
    std::ofstream fout;
    short command = 0;
    std::vector<std::string> menu;
    menu.emplace_back(" Save the report in the project folder ");
    menu.emplace_back(" Specify save path ");
    if (!printMenu(menu, command)) return;
    if (command == 1)
    {
        while (true)
        {
            setConsoleCursorPositionAndColor(0, 1, WHITE << 4 | BLACK);
            std::cout << " Specify save path: " << std::endl;
            std::string str;
            if (!inputField([&str](const std::string& s) { str = s; }, 256, 0, 2, -1))
                return;

            if (str[str.length() - 1] != '\\' && str[str.length() - 1] != '/') str += '\\';
            fout.open(str + "Project" + std::to_string(ID) + "_Report.txt");
            if (!fout.is_open() || fout.fail())
            {
                setConsoleCursorPositionAndColor(0, 3, BLACK << 4 | RED);
                std::cout << "The path is incorrect. Try again.";
                continue;
            }
            break;
        }
    }
    else fout.open(std::to_string(ID) + "\\Report.txt");
    fout << "Project ID: " << ID << std::endl;
    fout << "Project name: " << name << std::endl;
    fout << "Project description: " << std::endl << description << std::endl;
    fout << "Project creation date: " << dateCreated << std::endl << std::endl;
    fout << "Project members in total: " << projectMembers.size() << ". Of which:" << std::endl;
    short count1 = 0, count2 = 0, count3 = 0, count4 = 0;
    for (short i = 0; i < projectMembers.size(); i++)
        if (projectMembers[i]->getRole() == "Administrator") count1++;
        else if (projectMembers[i]->getRole() == "Client / Designer") count2++;
        else if (projectMembers[i]->getRole() == "Developer") count3++;
        else count4++;
    fout << "\tadministrators: " << count1 << std::endl;
    fout << "\tclients / designers: " << count2 << std::endl;
    fout << "\tdevelopers: " << count3 << std::endl;
    fout << "\ttesters: " << count4 << std::endl << std::endl;
    fout << "Project issues in total: " << Issue::getIssuesCount() << ". Of which:" << std::endl;
    count1 = 0;
    count2 = 0;
    count3 = 0;
    count4 = 0;
    for (short i = 0; i < toDoList.size(); i++)
        if (toDoList[i]->getType() == FEATURE)
        {
            std::shared_ptr<Feature> feat = std::static_pointer_cast<Feature>(toDoList[i]);
            count1 += feat->issues.size();
        }
    count1 += toDoList.size();
    fout << "\tto do: " << count1 << std::endl;
    for (short i = 0; i < inProgress.size(); i++)
        if (inProgress[i]->getType() == FEATURE)
        {
            std::shared_ptr<Feature> feat = std::static_pointer_cast<Feature>(inProgress[i]);
            count2 += feat->issues.size();
        }
    count2 += inProgress.size();
    fout << "\tin progress: " << count2 << std::endl;
    for (short i = 0; i < inReview.size(); i++)
        if (inReview[i]->getType() == FEATURE)
        {
            std::shared_ptr<Feature> feat = std::static_pointer_cast<Feature>(inReview[i]);
            count3 += feat.get()->issues.size();
        }
    count3 += inReview.size();
    fout << "\tin review: " << count3 << std::endl;
    for (short i = 0; i < done.size(); i++)
        if (done[i]->getType() == FEATURE)
        {
            std::shared_ptr<Feature> feat = std::static_pointer_cast<Feature>(done[i]);
            count4 += feat->issues.size();
        }
    count4 += done.size();
    fout << "\tresolved: " << count4 << std::endl;
    fout.close();
    setConsoleCursorPositionAndColor(0, 3, BLACK << 4 | GREEN);
    std::cout << " Report successfully saved to a text file.";
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
}

void Project::sortIssues()
{
    SetConsoleTextAttribute(handle, BLACK << 4 | WHITE);
    system("cls");
    std::vector<std::string> menu;
    menu.emplace_back(" Sort to do issues ");
    menu.emplace_back(" Sort issues in progress ");
    menu.emplace_back(" Sort issues in review ");
    menu.emplace_back(" Sort resolved issues ");
    menu.emplace_back(" Sort all issues ");
    short listTypeChoice = 0;
    if (!printMenu(menu, listTypeChoice)) return;
    SetConsoleTextAttribute(handle, BLACK << 4 | WHITE);
    system("cls");
    menu.clear();
    menu.emplace_back(" Sort by priority (descending) ");
    menu.emplace_back(" Sort by priority (ascending) ");
    menu.emplace_back(" Sort by ID (descending) ");
    menu.emplace_back(" Sort by ID (ascending) ");
    menu.emplace_back(" Sort by creation date (descending) ");
    menu.emplace_back(" Sort by creation date (ascending) ");
    short sortTypeChoice = 0;
    if (!printMenu(menu, sortTypeChoice)) return;
    SetConsoleTextAttribute(handle, BLACK << 4 | WHITE);
    system("cls");
    if (listTypeChoice == 0 || listTypeChoice == 4)
    {
        sortExactList(toDoList, sortTypeChoice);
        saveList(toDoList, std::to_string(ID) + "\\ToDoList");
    }
    if (listTypeChoice == 1 || listTypeChoice == 4)
    {
        sortExactList(inProgress, sortTypeChoice);
        saveList(inProgress, std::to_string(ID) + "\\InProgress");
    }
    if (listTypeChoice == 2 || listTypeChoice == 4)
    {
        sortExactList(inReview, sortTypeChoice);
        saveList(inReview, std::to_string(ID) + "\\InReview");
    }
    if (listTypeChoice == 3 || listTypeChoice == 4)
    {
        sortExactList(done, sortTypeChoice);
        saveList(done, std::to_string(ID) + "\\Done");
    }
    setConsoleCursorPositionAndColor(0, 0, BLACK << 4 | GREEN);
    std::cout << " Sorting completed successfully.";
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
}

void Project::sortExactList(std::vector<std::shared_ptr<Issue>>& list, short sortTypeChoice)
{
    for (short i = 0; i < list.size(); i++)
        if (list[i]->getType() == FEATURE)
        {
            std::shared_ptr<Feature> feat = std::static_pointer_cast<Feature>(list[i]);
            sortExactList(feat->issues, sortTypeChoice);
            feat->saveFeatureSubtasks();
        }
    switch (sortTypeChoice)
    {
    case 0:
        sort(list.begin(), list.end(),
             [](std::shared_ptr<Issue> issue1, std::shared_ptr<Issue> issue2)
             {
                 return issue1->getPriority() > issue2->getPriority();
             });
        break;
    case 1:
        sort(list.begin(), list.end(),
             [](std::shared_ptr<Issue> issue1, std::shared_ptr<Issue> issue2)
             {
                 return issue1->getPriority() < issue2->getPriority();
             });
        break;
    case 2:
        sort(list.begin(), list.end(),
             [](std::shared_ptr<Issue> issue1, std::shared_ptr<Issue> issue2)
             {
                 return issue1->issueData.ID > issue2->issueData.ID;
             });
        break;
    case 3:
        sort(list.begin(), list.end(),
             [](std::shared_ptr<Issue> issue1, std::shared_ptr<Issue> issue2)
             {
                 return issue1->issueData.ID < issue2->issueData.ID;
             });
        break;
    case 4:
        sort(list.begin(), list.end(),
             [](std::shared_ptr<Issue> issue1, std::shared_ptr<Issue> issue2)
             {
                 return compareDates(issue1->getDateCreated(), issue2->getDateCreated());
             });
        break;
    case 5:
        sort(list.begin(), list.end(),
             [](std::shared_ptr<Issue> issue1, std::shared_ptr<Issue> issue2)
             {
                 return !compareDates(issue1->getDateCreated(), issue2->getDateCreated());
             });
        break;
    }
}
