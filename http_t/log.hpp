#pragma once

#include <iostream>
#include <string>

enum log
{
    NORMAL,
    DEBUG,
    ERROR,
    WRONG,
    FATAL
};

inline void LOG(log level, const std::string& msg) { std::cout << msg << std::endl; }