#pragma once

#include <iosfwd>

// A template class for storing two different objects
template <class T, class T2>
class Pair
{
    T first;
    T2 second;

public:
    T getFirst() { return first; }
    T2 getSecond() { return second; }
    void setFirst(T first) { this->first = first; }
    void setSecond(T2 second) { this->second = second; }
    template <class T, class T2>
    friend std::ostream& operator<<(std::ostream&, Pair<T, T2>&);

    Pair(T first, T2 second)
    {
        this->first = first;
        this->second = second;
    }

    Pair(const Pair& pair)
    {
        this->first = pair.first;
        this->second = pair.second;
    }
};

template <class T, class T2>
std::ostream& operator<<(std::ostream& stream, Pair<T, T2>& pair)
{
    stream << pair.second;
    return stream;
}
