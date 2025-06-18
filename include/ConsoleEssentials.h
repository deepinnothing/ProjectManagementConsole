#pragma once

#include <conio.h>
#include <functional>
#include <iostream>
#include <windows.h>
#include <limits>
#include <vector>
#include <memory>
#include <string>
class Issue;
class ProjectMember;

#undef max

// Variables and functions for easier console operation and a custom namespace
// 'ace' stands for additional console essentials
namespace ace
{
    inline COORD coord;
    inline HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);

    enum colors
    {
        BLACK, BLUE, GREEN, CYAN, RED, PURPLE, BROWN, LIGHTGREY, DARKGREY, LIGTHBLUE, LIGHTGREEN, LIGHTCYAN, LIGHTRED,
        LIGHTPURPLE, YELLOW, WHITE
    };

    inline char getchChar;

    void setConsoleCursorPositionAndColor(short X, short Y, short color);

    void clearMessage(short Y);

    void updateConsoleCursorPosition();

    // Template function for menu display (print list of similar values and possibility to select a certain item)
    template <class T>
    bool printMenu(std::vector<T>& menu, short& command, short startPos = 0)
    {
        for (short i = 0; i < menu.size(); i++)
            std::cout << menu[i] << std::endl;
        while (true)
        {
            for (short i = 0; i < menu.size(); i++)
            {
                if (i == command) SetConsoleTextAttribute(handle, WHITE << 4 | BLACK);
                else SetConsoleTextAttribute(handle, BLACK << 4 | WHITE);
                if (i > command - 2 && i < command + 2)
                {
                    SetConsoleCursorPosition(handle, {0, static_cast<short>(i + startPos)});
                    std::cout << menu[i] << std::endl;
                }
            }
            setConsoleCursorPositionAndColor(0, command + startPos, WHITE << 4 | BLACK);
            getchChar = _getch();
            if (getchChar == 27) return false;
            if (getchChar == -32)
            {
                switch (_getch())
                {
                case 72: if (command != 0) command--;
                    break;
                case 80: if (command != menu.size() - 1) command++;
                    break;
                }
            }
            if (getchChar == '\r') return true;
        }
    }

    // Overloaded printMenu function for printing issues
    bool printMenu(std::vector<std::shared_ptr<Issue>>& issues, short& issueChoice, const std::string& lastOption = "",
                   short count1 = std::numeric_limits<short>::max(), short count2 = std::numeric_limits<short>::max(),
                   short count3 = std::numeric_limits<short>::max());

    void printIssueTableHeader();

    void printIssuesWithStatuses(std::vector<std::shared_ptr<Issue>>& issues,
                                 short count1 = std::numeric_limits<short>::max(),
                                 short count2 = std::numeric_limits<short>::max(),
                                 short count3 = std::numeric_limits<short>::max(), short i = -1);

    // printMenu function overload for printing out project members
    bool printMenu(std::vector<std::shared_ptr<ProjectMember>>& projectMembers, short& command, short startPos = 0);

    bool inputField(std::function<void(const std::string&)> setter, short strMaxLength, short X, short Y,
                    short messageY, bool bMultiLine = false, bool bPassword = false);
}
