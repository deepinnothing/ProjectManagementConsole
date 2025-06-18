#pragma once
#include <stdexcept>
#include <string>
#include <ctime>

inline std::string dateCurrent()
{
    std::string date;
    time_t now = time(0);
    tm* ltm = new tm;
    localtime_s(ltm, &now);
    if (ltm->tm_mday < 10) date = '0';
    date += std::to_string(ltm->tm_mday) + '.';
    if (ltm->tm_mon < 10) date += '0';
    date += std::to_string(ltm->tm_mon + 1) + '.' + std::to_string(ltm->tm_year + 1900) + '\0';
    return date;
}

inline void checkDate(std::string date)
{
    if (date.length() != 10)
        throw std::invalid_argument(
            "Incorrect date format. The input format should correspond to DD.MM.YYYY");
    if (date[3] == '0' && date[4] == '2')
    {
        std::string s_year;
        short year;

        for (short i = 0; i < 4; i++) s_year.push_back(date[6 + i]);
        year = stoi(s_year);
        if (date[1] == '9' && (year % 4 != 0 || (year % 100 == 0 && year % 400 != 0)))
            throw std::invalid_argument("The year is not a leap year");
        if (date[0] > '2') throw std::invalid_argument("There can be no more than 29 days in February");
    }
    else
    {
        if (date[3] > '1' || (date[3] == '1' && date[4] > '2') || (date[3] == '0' && date[4] == '0'))
            throw std::invalid_argument("Incorrect month number");
        if (date[0] > '3' || (date[0] == '0' && date[1] == '0') || (date[0] == '3' &&
            ((date[1] > '0' && (date[4] == '4' || date[4] == '6' || date[4] == '9' || (date[3] == '1' && date[4] ==
                '1'))) || date[1] > '1')))
            throw std::invalid_argument("Incorrect day number");
        if (date[0] < '0' || date[0] > '9' || date[1] < '0' || date[1] > '9' || date[2] != '.' ||
            date[3] < '0' || date[3] > '9' || date[4] < '0' || date[4] > '9' || date[5] != '.' ||
            date[6] < '0' || date[6] > '9' || date[7] < '0' || date[7] > '9' || date[8] < '0' || date[8] > '9' || date[
                9] < '0' || date[9] > '9')
            throw std::invalid_argument("Incorrect date format. The input format should correspond to DD.MM.YYYY");
    }
}

inline bool compareDates(std::string date1, std::string date2)
{
    if (stoi(date1.substr(6, 4)) != stoi(date2.substr(6, 4)))
        return stoi(date1.substr(6, 4)) > stoi(date2.substr(6, 4));
    if (stoi(date1.substr(3, 2)) != stoi(date2.substr(3, 2)))
        return stoi(date1.substr(3, 2)) > stoi(date2.substr(3, 2));
    return stoi(date1.substr(0, 2)) > stoi(date2.substr(0, 2));
}
