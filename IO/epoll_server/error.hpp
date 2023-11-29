#pragma once


enum ERR
{
    USAGE_ERR = 1,
    SOCK_ERR,
    BIND_ERR,
    LISTEN_ERR,
    CONNECT_ERR,
    EPOLL_CREATE_ERR
};