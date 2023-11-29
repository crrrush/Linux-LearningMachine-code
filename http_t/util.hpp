#pragma once

#include <iostream>
#include <fstream>
#include <string>

class util
{
public:
    // util(/* args */);
    // ~util();

    static std::string getoneline(std::string &s, const std::string &sep)
    {
        size_t pos = s.find(sep);
        if (pos == std::string::npos)
            return "";
        return s.substr(0, pos);
    }

    static bool loadsource(const std::string &source, std::string &out)
    {
        std::ifstream in(source, std::ios::binary);
        if (!in.is_open())
            return false;

        std::string buffer;
        while (std::getline(in, buffer))
        {
            out += buffer;
        }

        in.close();
        return true;
    }

    static std::string sufftodesc(const std::string &sf)
    {
        std::string dc = "Content-Type: ";
        if(sf == ".html") dc += "text/html";
        else if(sf == ".jpg") dc += "image/jpeg";

        dc += "\r\n";
        return dc;
    }
};
