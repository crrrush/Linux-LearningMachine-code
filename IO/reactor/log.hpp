#pragma once

#include <iostream>
#include <string>
#include <cstdio>
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
    // if (lv < DEFAULT_LEVEL)
    //     return;

#define NUM 1024
    char logprefix[NUM];
    time_t t = time(nullptr);
    struct tm *lt = localtime(&t);
    char tmbuf[32] = {0};
    strftime(tmbuf, 31, "%H:%M:%S", lt);
    snprintf(logprefix, sizeof(logprefix), "[%s][%s] ", level_str(lv), tmbuf);

    char logcontent[NUM];
    va_list args;
    va_start(args, format);
    vsnprintf(logcontent, sizeof(logcontent), format, args);

    std::cout << logprefix << logcontent << std::endl;
}

/*

// #define LOG(lev, format, ...)                  \
//     do                                         \
//     {                                          \
//         if (lev < DEFAULT_LEVEL)               \
//             break;                             \
//         printf("[%s:%d]", __FILE__, __LINE__); \
//         log(lev, format, ##__VA_ARGS__);       \
//     }while(0)

*/