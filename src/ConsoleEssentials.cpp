#include "ConsoleEssentials.h"
#include "Issue.h"
#include "ProjectMember.h"

#include <iomanip>

#undef max

void ace::setConsoleCursorPositionAndColor(short X, short Y, short color)
{
    coord.X = X;
    coord.Y = Y;
    SetConsoleCursorPosition(handle, coord);
    SetConsoleTextAttribute(handle, color);
}

void ace::clearMessage(short Y)
{
    setConsoleCursorPositionAndColor(0, Y, BLACK << 4 | WHITE);
    for (short i = 0; i < 150; i++) std::cout << ' ';
}

void ace::updateConsoleCursorPosition()
{
    CONSOLE_SCREEN_BUFFER_INFO cbsi;
    if (GetConsoleScreenBufferInfo(handle, &cbsi))
        coord = cbsi.dwCursorPosition;
}

void ace::printIssueTableHeader()
{
    std::cout << std::left << std::setw(6) << "  ID" << '|' << std::setw(8) << "  Type" << '|' << std::setw(26) <<
        "           Name" << '|' << std::setw(8) << "Priority" << '|' << std::setw(13) << "Creation date" << '|' <<
        std::setw(12) << "Resolve date" << '|' << std::setw(8) << "Subtasks" << '|' << std::setw(26) <<
        "        Created by" << '|' << std::setw(27) << "        Assigned to" << '|' << std::setw(11) << "  Status"
        << std::endl;
}

void ace::printIssuesWithStatuses(std::vector<std::shared_ptr<Issue>>& issues, short count1, short count2, short count3,
                                  short i)
{
    short iterations = issues.size();
    if (i > -1) iterations = i + 1;
    else i = 0;

    for (; i < iterations; i++)
    {
        issues[i]->print();
        if (i >= count1 + count2 + count3) std::cout << std::setw(11) << "Resolved" << std::endl;
        else if (i >= count1 + count2) std::cout << std::setw(11) << "In review" << std::endl;
        else if (i >= count1) std::cout << std::setw(11) << "In progress" << std::endl;
        else std::cout << std::setw(11) << "To do" << std::endl;
    }
}


// Overloaded printMenu function for printing issues
bool ace::printMenu(std::vector<std::shared_ptr<Issue>>& issues, short& issueChoice, const std::string& lastOption,
                    short count1, short count2, short count3)
{
    printIssueTableHeader();
    printIssuesWithStatuses(issues, count1, count2, count3);
    if (!lastOption.empty()) std::cout << lastOption << std::endl;
    while (true)
    {
        // Focusing on a specific issue
        setConsoleCursorPositionAndColor(0, 1, WHITE << 4 | BLACK);
        for (short i = 0; i <= issues.size(); i++)
        {
            if (i == issueChoice) SetConsoleTextAttribute(handle, WHITE << 4 | BLACK);
            else SetConsoleTextAttribute(handle, BLACK << 4 | WHITE);
            if (i > issueChoice - 2 && i < issueChoice + 2)
            {
                SetConsoleCursorPosition(handle, {0, static_cast<short>(i + 1)});
                if (i == issues.size() && !lastOption.empty())
                    std::cout << lastOption << std::endl;
                else if (i < issues.size())
                    printIssuesWithStatuses(issues, count1, count2, count3, i);
            }
        }
        coord.Y = issueChoice + 1;
        SetConsoleCursorPosition(handle, coord);
        getchChar = _getch();
        if (getchChar == 27) return false;
        if (getchChar == '\r') break;
        if (getchChar == -32)
        {
            switch (_getch())
            {
            case 72: if (issueChoice != 0) issueChoice--;
                break;
            case 80: if (issueChoice != issues.size() - 1) issueChoice++;
                break;
            }
        }
    }
    return true;
}

bool ace::printMenu(std::vector<std::shared_ptr<ProjectMember>>& projectMembers, short& command, short startPos)
{
    std::cout << std::left << std::setw(10) << "    ID" << '|' << std::setw(20) << "        Login" << '|' <<
        std::setw(20) << "        Name" << '|' << std::setw(20) << "       Surname" << '|' << std::setw(20) <<
        "     Project role" << std::endl;
    for (short i = 0; i < projectMembers.size(); i++)
        std::cout << *projectMembers[i] << std::endl;
    while (true)
    {
        for (short i = 0; i < projectMembers.size(); i++)
        {
            if (i == command) SetConsoleTextAttribute(handle, WHITE << 4 | BLACK);
            else SetConsoleTextAttribute(handle, BLACK << 4 | WHITE);
            if (i > command - 2 && i < command + 2)
            {
                SetConsoleCursorPosition(handle, {0, static_cast<short>(i + 1)});
                std::cout << *projectMembers[i] << std::endl;
            }
        }
        setConsoleCursorPositionAndColor(0, command, WHITE << 4 | BLACK);
        getchChar = _getch();
        if (getchChar == 27) return false;
        if (getchChar == -32)
        {
            switch (_getch())
            {
            case 72: if (command != startPos) command--;
                break;
            case 80: if (command != projectMembers.size() - 1) command++;
                break;
            }
        }
        if (getchChar == '\r') return true;
    }
}

bool ace::inputField(std::function<void(const std::string&)> setter, short strMaxLength, short X, short Y,
                     short messageY, bool bMultiLine, bool bPassword)
{
    std::string str;
    //getchChar = _getch();

    while (true)
    {
        if (bMultiLine) setConsoleCursorPositionAndColor(0, Y, WHITE << 4 | BLACK);
        else setConsoleCursorPositionAndColor(X + str.length(), Y, WHITE << 4 | BLACK);
        if (bMultiLine) std::cout << str;
        getchChar = _getch();

        if (getchChar == 27) return false;

        // Processing Return/Enter key press
        if (getchChar == '\r')
        {
            try
            {
                setter(str);
                break;
            }
            catch (std::exception& ex)
            {
                if (bMultiLine)
                {
                    updateConsoleCursorPosition();
                    short prevY = coord.Y;
                    if (messageY > -1) clearMessage(messageY);
                    messageY = prevY + 1;
                }
                clearMessage(messageY);
                setConsoleCursorPositionAndColor(0, messageY, BLACK << 4 | RED);
                std::cerr << ex.what();
            }
        }

        // Processing Backspace key press
        else if (getchChar == 8 && !str.empty())
        {
            SetConsoleTextAttribute(handle, BLACK << 4 | WHITE);
            std::cout << "\b \b";
            str.pop_back();
        }

        // Processing any other key press
        else if (getchChar != 8 && str.length() != strMaxLength)
        {
            if (getchChar > -1 && getchChar < 32 && ((bMultiLine && getchChar != '\n') || !bMultiLine))
            {
                while (_kbhit()) _getch();
                continue;
            }
            str.push_back(getchChar);
            if (!bMultiLine) std::cout.put(bPassword ? '*' : getchChar);
        }
    }

    clearMessage(messageY);
    return true;
}
