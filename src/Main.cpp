// Program startup file with includes of necessary libraries and files
#include<iostream>
#include<string>
#include<conio.h>
#include<filesystem>
#include<fstream>
#include<vector>
#include<windows.h>

#include "Account.h"
#include "ProjectMember.h"
#include "Project.h"
#include "ConsoleEssentials.h"
using namespace ace;

int main()
{
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
    std::cin.unsetf(std::ios::skipws);

    std::cout << "THE PROGRAM IS BEST RUN IN FULLSCREEN MODE TO MINIMIZE CONSOLE DISPLAYING PROBLEMS" << std::endl;
    SetConsoleTextAttribute(handle, WHITE << 4 | BLACK);
    std::cout << "Press any key to continue...";
    _getch();
    
    if (!Account::currentAccount->loginPrompt()) return 0;

    // Main menu
    while (true)
    {
        // Set black background and white foreground
        SetConsoleTextAttribute(handle, BLACK << 4 | WHITE);
        system("cls");
        
        short command = 0;
        std::vector<std::string> menu;
        menu.emplace_back(" Choose an existing project ");
        menu.emplace_back(" Create a new project ");
        menu.emplace_back(" Quit ");
        if (!printMenu(menu, command)) return 0;
        if (command == 2) return 0;

        // Choosing an existing project
        if (command == 0)
        {
            // Reading existing projects
            std::vector<std::shared_ptr<Project>> projects;
            if (std::filesystem::exists("Projects"))
            {
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

                    // Read project-related details
                    std::string projectFolder = std::to_string(readID) + '\\';
                    std::ifstream fin_r(projectFolder + "ProjectMembers", std::ios::binary);
                    char readRole[21];
                    while (fin_r.read(reinterpret_cast<char*>(&readID), sizeof(readID)))
                    {
                        // Only the projects current user is member of should be available
                        if (readID == Account::currentAccount->getID())
                        {
                            projects.push_back(readProject);
                            break;
                        }
                        fin_r.read(reinterpret_cast<char*>(&readRole), sizeof(readRole));
                    }
                }
            }
            
            SetConsoleTextAttribute(handle, BLACK << 4 | WHITE);
            system("cls");
            
            if (projects.empty())
            {
                std::cout <<
                    "You don't have any projects yet. Create a new one or wait for other projects' administrators to grant you access and start tracking bugs and tasks that occur during project development."
                    << std::endl;
                short command = 0;
                menu.clear();
                menu.emplace_back(" Create a new project ");
                menu.emplace_back(" Quit ");
                updateConsoleCursorPosition();
                if (!printMenu(menu, command, coord.Y)) return 0;
                if (command == 1) return 0;
                Project::currentProject->create();
            }
            else
            {
                std::cout << std::left << std::setw(10) << "    ID" << '|' << std::setw(40) << "                Project name" << '|' <<
                    std::setw(13) << "Creation date" << std::endl;
                short projectChoice = 0;
                while (true)
                {
                    // Focusing on a specific project
                    setConsoleCursorPositionAndColor(0, 1, WHITE << 4 | BLACK);
                    for (short i = 0; i < projects.size(); i++)
                    {
                        if (i == projectChoice) SetConsoleTextAttribute(handle, WHITE << 4 | BLACK);
                        else SetConsoleTextAttribute(handle, BLACK << 4 | WHITE);
                        std::cout << std::left << std::setw(10) << projects[i]->getID() << '|';
                        if (projects[i]->getName().length() > 40) std::cout << std::setw(40) << projects[i]->getName().
                            substr(37) + "...";
                        else std::cout << std::setw(40) << projects[i]->getName();
                        std::cout << '|' << std::setw(13) << ' ' + projects[i]->getDateCreated() << std::endl;
                    }
                    coord.Y = projectChoice + 1;
                    SetConsoleCursorPosition(handle, coord);

                    // Get pressed key
                    getchChar = _getch();
                    if (getchChar == 27 || getchChar == '\r') break;

                    // Handle arrow keys
                    if (getchChar == -32)
                    {
                        switch (_getch())
                        {
                        case 72: if (projectChoice != 0) projectChoice--;
                            break;
                        case 80: if (projectChoice != projects.size() - 1) projectChoice++;
                            break;
                        }
                    }
                }

                // If esc was pressed, go back to the main menu
                if (getchChar == 27) continue;

                // If return was pressed, load the project
                Project::currentProject = projects[projectChoice];
                std::string projectFolder = std::to_string(Project::currentProject->getID()) + '\\';
                std::ifstream fin_r(projectFolder + "ProjectMembers", std::ios::binary);
                unsigned readID;
                char readRole[21];
                while (fin_r.read(reinterpret_cast<char*>(&readID), sizeof(readID)))
                {
                    fin_r.read(reinterpret_cast<char*>(&readRole), sizeof(readRole));
                    if (readID == Account::currentAccount->getID()) break;
                }
                fin_r.close();
                ProjectMember::currentProjectMember->setRole(readRole);
                ProjectMember::currentProjectMember->setAccount(Account::currentAccount);
                Project::currentProject->loadProject();
                Project::currentProject->actions();
            }
        }

        // Creating a new project
        else Project::currentProject->create();
    }
}
