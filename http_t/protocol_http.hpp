#pragma once

#include <sstream>
#include <string>

#include "util.hpp"

// http.conf 配置文件
// 读取 初始化服务器

const std::string sep = "\r\n";
const std::string default_path = "./wwwroot";
const std::string homepage = "index.html";
const std::string notfound = "./wwwroot/404.html";
const std::string blank = "\r\n";


class httpRequest
{
public:
    // httpRequest(/* args */){}
    // ~httpRequest(){}

    void parse()
    {
        // 1.
        // 拿第一行 方法 url 版本
        std::string line = util::getoneline(_inbuffer, sep);

        // 以空格为间隔分割
        std::stringstream ss(line);
        ss >> _method >> _url >> _version;

        // 文件路径
        _path = default_path;
        _path += _url;
        if(_path.back() == '/') _path += homepage;

        // 后缀 资源类型
        auto pos = _path.rfind('.');
        if(pos == std::string::npos) _suffix = ".html";
        else _suffix = _path.substr(pos);

    }

public:/* data */
    std::string _method;
    std::string _url;
    std::string _version;
    std::string _path;
    std::string _suffix;

    std::string _inbuffer;
};



class httpResponse
{
public:
    // httpResponse(/* args */){}
    // ~httpResponse(){}

public:/* data */
    std::string _outbuffer;
};
