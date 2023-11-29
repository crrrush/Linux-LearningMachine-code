#pragma once

#include <iostream>
#include <string>
#include <cstdarg>
#include <ctime>
#include <unistd.h>

enum level
{
    DEBUG,
    NORMAL,
    WARNING,
    ERROR,
    FATAL
};

#define DEFAULT_LEVEL DEBUG

// inline void LOG(level lv, const std::string& msg) { std::cout << msg << std::endl; }

const char *level_str(level lv)
{
    switch (lv)
    {
    case DEBUG:
        return "DEBUG";
        break;
    case NORMAL:
        return "NORMAL";
        break;
    case WARNING:
        return "WARNING";
        break;
    case ERROR:
        return "ERROR";
        break;
    case FATAL:
        return "FATAL";
        break;
    default:
        return nullptr;
        break;
    }
}

void log(level lv, const char *format, ...)
{
    if (lv < DEFAULT_LEVEL)
        return;

#define NUM 1024
    char logprefix[NUM];
    snprintf(logprefix, sizeof(logprefix), "[%s][%ld] ", level_str(lv), (long int)time(nullptr));

    char logcontent[NUM];
    va_list args;
    va_start(args, format);
    vsnprintf(logcontent, sizeof(logcontent), format, args);

    std::cout << logprefix << logcontent << std::endl;
}