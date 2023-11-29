#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <cstring>
#include <arpa/inet.h>
#include <jsoncpp/json/json.h>

#include "log.hpp"

#include "cal.hpp"

// 报文
// requset
// "contentlength x y op\r\n"
// "x y op\r\n"
// response
// "contentlength num exitcode\r\n"
// "resultnum exitcode\r\n"

// struct request{
// int x;
// int y;
// char op;
// }
//
// struct response{
// int num;
// int exitcode;
// }
//

#define SEP ' '
#define END "\r\n"
#define ENDLEN strlen(END)

std::string decode(const std::string &msg)
{
    auto it = msg.begin();
    while (*it != '\r')
        ++it;
    return std::string(msg.begin(), it);
}

void encode(std::string &msg)
{
    msg += END;
}

class request
{
public:
    request(/* args */) {}
    ~request() {}

    // "x y op"
    std::string serialization(const equation &e)
    {
        std::string se;
#ifdef MYSELF
        se += std::to_string(e._x);
        se += ' ';
        se += std::to_string(e._y);
        se += ' ';
        se += e._op;

#else
        Json::Value root;
        root["x"] = e._x;
        root["y"] = e._y;
        root["op"] = e._op;

        Json::FastWriter writer;
        se = writer.write(root);
#endif
        return se;
    }
    // "x y op"
    equation deserialization(const std::string &req)
    {
#ifdef MYSELF
        std::string x;
        std::string y;

        size_t pos1 = req.find(SEP);
        x = req.substr(0, pos1++);
        size_t pos2 = req.find(SEP, pos1);
        y = req.substr(pos1, pos2 - pos1);
        // std::cout << "pos1: " << pos1 << " pos2: " << pos2 <<std::endl;

        // std::cout << "x: " << x << std::endl;
        // std::cout << "y: " << y << std::endl;

        _e._x = std::stoi(x);
        _e._y = std::stoi(y);
        _e._op = req.back();

#else
        Json::Value root;
        Json::Reader reader;
        reader.parse(req, root);

        _e._x = root["x"].asInt();
        _e._y = root["y"].asInt();
        _e._op = root["op"].asInt();
#endif

        return _e;
    }

    result cal() { return calserver::calculator(_e); }

private: /* data */
    equation _e;
};

class response
{
public:
    response(/* args */) {}
    response(const result &r) : _r(r) {}
    ~response() {}

    // "resultnum exitcode"
    std::string serialization()
    {
        std::string se;
#ifdef MYSELF
        se += std::to_string(_r._num);
        se += ' ';
        se += std::to_string((int)_r._code);

#else
        Json::Value root;
        root["num"] = _r._num;
        root["exitcode"] = _r._code;

        Json::FastWriter writer;
        se = writer.write(root);
#endif

        return se;
    }

    // "resultnum exitcode"
    result deserialization(const std::string &resp)
    {
#ifdef MYSELF
        std::string num;

        size_t pos = resp.find(SEP);
        num = resp.substr(0, pos);
        // std::cout << "num: " << num <<std::endl;

        _r._code = (EXITCODE)(resp.back() - '0');
        _r._num = std::stoi(num);

#else
        Json::Value root;
        Json::Reader reader;
        reader.parse(resp, root);

        _r._num = root["num"].asInt();
        _r._code = (EXITCODE)root["exitcode"].asInt();
#endif

        return _r;
    }

private: /* data */
    result _r;
};

bool recvmessage(std::string &in, std::string *out) // 获取一个完整报文
{
    size_t end = in.find(END);
    if (end == std::string::npos)
        return false;

    *out = in.substr(0, end + ENDLEN);
    in.erase(0, end + ENDLEN);

    return true;
}

// 报文样式 继承？
// size data   (size_t equation)
// size data   (size_t result)
//

// class request
// {
// public:
//     request(/* args */) {}
//     ~request() {}

//     static void pack(std::vector<char>* out){}
//     static request unpack(const std::vector<char>& in)
//     {

//     }

//     void encode() {}
//     void decode() {}

//     void serialization() {}
//     void deserialization() {}

// private: /* data */
//     size_t _size;
//     equation _e;
// };

// class response
// {
// public:
//     response(/* args */) {}
//     ~response() {}

//     void encode() {}
//     void decode() {}

//     void serialization() {}
//     void deserialization() {}

// private: /* data */
//     size_t _size;
//     result _r;
// };

// union to_sz
// {
//     char _a;
//     char _b;
//     char _c;
//     char _d;
//     uint32_t _self;
// };

// bool load(const int &fd, std::queue<char> &buffer)
// {
// #define BUFFERSIZE 1024
//     // 直接读 ?
//     char tmpbuffer[BUFFERSIZE];
//     // std::cout << "waiting read\n";
//     ssize_t n = read(fd, tmpbuffer, sizeof(tmpbuffer));
//     if (-1 == n)
//     {
//         // log ERROR
//         return false;
//     }
//     else if (0 == n)
//     {
//         // log normal
//         std::cout << "client quit\n";
//         return false;
//     }

//     // 放入缓冲区
//     for (int i = 0; i <= n; ++i)
//         buffer.push(tmpbuffer[i]);
//     return true;
// }

// bool recvmessage(const int &fd, std::vector<char> &out, size_t sz)
// {

//     static std::queue<char> buffer;
//     while (true)
//     {
//         // 判断是否是完整报文
//         while (buffer.size() < sz) if(!load(fd, buffer)) return false;

//         // 取出报文
//         while (sz--)
//         {
//             out.push_back(buffer.front());
//             buffer.pop();
//         }
//         return true;
//     }
// }

// // bool recvmessage(const int &fd, std::vector<char> &out)
// // {

// //     static std::queue<char> buffer;
// //     while (true)
// //     {
// //         // 判断是否是完整报文
// //         while (buffer.size() < 4) if(!load(fd, buffer)) return false;

// //         to_sz s;
// //         s._a = buffer.front();
// //         out.push_back(buffer.front());
// //         buffer.pop();
// //         s._a = buffer.front();
// //         out.push_back(buffer.front());
// //         buffer.pop();
// //         s._a = buffer.front();
// //         out.push_back(buffer.front());
// //         buffer.pop();
// //         s._a = buffer.front();
// //         out.push_back(buffer.front());
// //         buffer.pop();
// //         size_t sz = ntohl(s._self);

// //         while (buffer.size() < sz) if(!load(fd, buffer)) return false;

// //         // 取出报文
// //         while (sz--)
// //         {
// //             out.push_back(buffer.front());
// //             buffer.pop();
// //         }
// //         return true;
// //     }
// // }
