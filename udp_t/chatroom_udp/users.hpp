#pragma once

#include <unordered_map>
#include <string>

class user
{
public:
    user(const std::string &ip, const uint16_t &port) : _ip(ip), _port(port) {}

    // private:
    std::string _ip;
    uint16_t _port;
};

class users
{
public:
    ~users() {}

    static users &GetInstance() { return _u; }

    void add(const std::string &ip, const uint16_t &port)
    {
        std::string id = ip + "-" + std::to_string(port);
        _map.insert(std::make_pair(id, user(ip, port)));
    }

    void del(const std::string &ip, const uint16_t &port)
    {
        std::string id = ip + "-" + std::to_string(port);
        _map.erase(id);
    }

    bool is_online(const std::string &ip, const uint16_t &port)
    {
        std::string id = ip + "-" + std::to_string(port);
        return _map.find(id) != _map.end();
    }

    void broadcast(const std::string &message, const string &ip, const uint16_t &port, const udpServer *ps)
    {
        // printf("broadcast in\n");
        std::string msg = "[" + ip + "-" + std::to_string(port) + "] #";
        msg += message;
        for (const auto &e : _map)
        {
            ps->send(msg, e.second._ip, e.second._port);
        }
        // printf("broadcast out\n");
    }

private:
    users() {}
    users(const users &u) = delete;
    users &operator=(const users &u) = delete;

private: /* data */
    std::unordered_map<std::string, user> _map;

    static users _u;
};

users users::_u;
