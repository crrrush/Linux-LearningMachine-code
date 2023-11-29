#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <mutex>
#include <memory>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define DICTPATH "./dict.txt"

class dict
{
public:
    ~dict() {}

    void load()
    {
        _ifs.open(DICTPATH, std::ifstream::in);

        std::string buffer;
        while (std::getline(_ifs, buffer))
        {
            size_t pos = buffer.find(' ');
            std::string org = buffer.substr(0, pos++);
            std::string tran = buffer.substr(pos);
            _dict[org] = tran;
        }

        _ifs.close();
    }

    const std::string find(const std::string& index)
    {
        auto it = _dict.find(index);
        if(it == _dict.end()) return "Not Found!";

        return it->second;
    }


    static dict &GetInstance()
    {
        if (!_pd)
        {
            std::lock_guard<std::mutex> guard(_mtx);
            if (!_pd)
                _pd = std::unique_ptr<dict>(new dict);
        }

        return *_pd;
    }

private:
    dict()
    {
        load();
    }

    // dict():_fd(-1)
    // {
    //     _fd = open(DICTPATH, O_RDONLY);
    //     char buffer[1024];
    //     ssize_t n = 1;
    //     while(n)
    //     {
    //     }
    //     close(_fd);
    //     _fd = -1;
    // }
    dict(const dict &d) = delete;
    dict &operator=(const dict &d) = delete;

private: /* data */
    // int _fd;
    std::ifstream _ifs;
    std::unordered_map<std::string, std::string> _dict;

    static std::unique_ptr<dict> _pd;
    static std::mutex _mtx;
};

std::mutex dict::_mtx;

std::unique_ptr<dict> dict::_pd;
