#pragma once

#include <iostream>
#include <vector>
#include <cstdio>
#include <ctime>
#include <cstdlib>
#include <unistd.h>

enum EXITCODE
{
    NORMOL,
    DEVZERO,
    MODZERO,
    OPWRONG,
    UNKNOWN
};

class calserver;

class equation
{
public:
    equation(int x = 0, int y = 0, char op = '*') : _x(x), _y(y), _op(op) {}

    equation(const calserver &cal);

    static char getOP(int n) { return _v[n % _v.size()]; }

    void out()
    {
        printf("%d %c %d == ? \n", _x, _op, _y);
    }

    int _x;
    int _y;
    char _op;

private:
    const static std::vector<char> _v;
};

struct result
{
    result(const int &num = 0, const EXITCODE &code = NORMOL) : _num(num), _code(code) {}

    void out()
    {
        switch (_code)
        {
        case NORMOL:
            printf("result: %d\n", _num);
            break;
        case DEVZERO:
            printf("除零错误！！！\n");
            break;
        case MODZERO:
            printf("模零错误！！！\n");
            break;
        case OPWRONG:
            printf("运算符错误！！！\n");
            break;
        case UNKNOWN:
            printf("未知错误！！！\n");
            break;
        default:
            break;
        }
    }

    int _num;
    EXITCODE _code;
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

    result operator()(const equation &e) const
    {
        switch (e._op)
        {
        case '+':
            return result(e._x + e._y);
            break;
        case '-':
            return result(e._x - e._y);
            break;
        case '*':
            return result(e._x * e._y);
            break;
        case '/':
        {
            if (e._y == 0)
                return result(0, DEVZERO);
            else
                return result(e._x / e._y);
        }
        break;
        case '%':
        {
            if (e._y == 0)
                return result(0, MODZERO);
            else
                return result(e._x % e._y);
        }
        break;
        default:
            return result(0, OPWRONG);
            break;
        }

        return result(0, UNKNOWN);
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
