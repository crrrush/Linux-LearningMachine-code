#pragma once

#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <unistd.h>

class calserver;

class equation
{
public:
    equation(int x = 0, int y = 0, char op = '*') : _x(x), _y(y), _op(op) {}

    equation(const calserver &cal);

    static char getOP(int n) { return _v[n % _v.size()]; }

    int _x;
    int _y;
    char _op;

private:
    const static std::vector<char> _v;
};

const std::vector<char> equation::_v = {'+', '-', '*', '/', '%'};

class calserver
{
public:
    ~calserver() {}

    static int randnum(int range)
    {
        return (rand() ^ time(nullptr) ^ 0x20230721) % range;
    }

    int operator()(const equation &e) const
    {
        switch (e._op)
        {
        case '+':
            return e._x + e._y;
            break;
        case '-':
            return e._x - e._y;
            break;
        case '*':
            return e._x * e._y;
            break;
        case '/':
        {
            if (e._y == 0)
            {
                std::cout << "除零错误\n";
                return 0;
            }
            return e._x / e._y;
        }
        break;
        case '%':
        {
            if (e._y == 0)
            {
                std::cout << "模零错误\n";
                return 0;
            }
            return e._x % e._y;
        }
        break;
        default:
            break;
        }

        std::cout << "error?\n";
        return 0;
    }

private:
    calserver()
    {
        srand((unsigned int)time(nullptr));
    }

    calserver(const calserver &cs) = delete;
    calserver &operator=(const calserver &cs) = delete;

public:
    static calserver calculator;
};

calserver calserver::calculator;

const static calserver &calculator = calserver::calculator;



equation::equation(const calserver &cal)
{
    _x = cal.randnum(100);
    _y = cal.randnum(100);
    _op = getOP(cal.randnum(100));
}
