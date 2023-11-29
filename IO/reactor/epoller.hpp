#pragma once

#include <cstring>
#include <cerrno>
#include <cstdlib>

#include <sys/epoll.h>
#include <unistd.h>

#include "error.hpp"
#include "log.hpp"

using epoll_event = struct epoll_event;
// typedef struct epoll_event epoll_event;

#define BLOCK -1

const static int _size = 128; // epoll_create函数参数

class epoller
{
public:
    epoller() : _epfd(-1) {}
    ~epoller()
    {
        if (_epfd != -1)
            close(_epfd);
    }

public:
    /////////////////////////////   create
    void create()
    {
        _epfd = epoll_create(_size);
        if (-1 == _epfd)
        {
            log(FATAL, "[epoll create failed][%d]: %s", errno, strerror(errno));
            exit(EPOLL_CREATE_ERR);
        }

        log(NORMAL, "epoll create successed");
    }

    /////////////////////////////   add
    bool add(const int &fd, epoll_event *event)
    {
        if (-1 == epoll_ctl(_epfd, EPOLL_CTL_ADD, fd, event))
        {
            log(ERROR, "[epoll add fd failed][fd : %d][%d]: %s", fd, errno, strerror(errno));
            return false;
        }

        log(NORMAL, "epoll add fd successed");
        return true;
    }

    bool add(const int &fd, const uint32_t &events, void *ptr_data)
    {
        epoll_event ev;
        ev.events = events;
        ev.data.ptr = ptr_data;
        if (-1 == epoll_ctl(_epfd, EPOLL_CTL_ADD, fd, &ev))
        {
            log(ERROR, "[epoll add fd failed][fd : %d][%d]: %s", fd, errno, strerror(errno));
            return false;
        }

        log(NORMAL, "epoll add fd successed");
        return true;
    }

    bool add(const int &fd, const uint32_t &events)
    {
        epoll_event ev;
        ev.events = events;
        ev.data.fd = fd;
        if (-1 == epoll_ctl(_epfd, EPOLL_CTL_ADD, fd, &ev))
        {
            log(ERROR, "[epoll add fd failed][fd : %d][%d]: %s", fd, errno, strerror(errno));
            return false;
        }

        log(NORMAL, "epoll add fd successed");
        return true;
    }

    // bool add(const int &fd, const uint32_t &events, const int &fd_data)
    // {
    //     epoll_event ev;
    //     ev.events = events;
    //     ev.data.fd = fd_data;
    //     if (-1 == epoll_ctl(_epfd, EPOLL_CTL_ADD, fd, &ev))
    //     {
    //         log(ERROR, "[epoll add fd failed][%d]: %s", errno, strerror(errno));
    //         return false;
    //     }

    //     log(NORMAL, "epoll add fd successed");
    //     return true;
    // }

    bool add(const int &fd, const uint32_t &events, const uint32_t &u32_data)
    {
        epoll_event ev;
        ev.events = events;
        ev.data.u32 = u32_data;
        if (-1 == epoll_ctl(_epfd, EPOLL_CTL_ADD, fd, &ev))
        {
            log(ERROR, "[epoll add fd failed][fd : %d][%d]: %s", fd, errno, strerror(errno));
            return false;
        }

        log(NORMAL, "epoll add fd successed");
        return true;
    }

    bool add(const int &fd, const uint32_t &events, const uint64_t &u64_data)
    {
        epoll_event ev;
        ev.events = events;
        ev.data.u64 = u64_data;
        if (-1 == epoll_ctl(_epfd, EPOLL_CTL_ADD, fd, &ev))
        {
            log(ERROR, "[epoll add fd failed][fd : %d][%d]: %s", fd, errno, strerror(errno));
            return false;
        }

        log(NORMAL, "epoll add fd successed");
        return true;
    }

    /////////////////////////////   del
    bool del(int fd)
    {
        // epoll模型中的红黑树是以fd为key值的，当删除红黑树节点时，需要关心节点内容是什么吗？
        // 显然是不需要的
        // 所以这里的epoll_event* 是被忽略的，可以直接传空指针
        if (-1 == epoll_ctl(_epfd, EPOLL_CTL_DEL, fd, nullptr))
        {
            log(ERROR, "[epoll delete fd failed][%d]: %s", errno, strerror(errno));
            return false;
        }

        log(NORMAL, "epoll delete fd successed");
        return true;
    }

    /////////////////////////////   mod
    bool mod(int fd, epoll_event *event)
    {
        if (-1 == epoll_ctl(_epfd, EPOLL_CTL_MOD, fd, event))
        {
            log(ERROR, "[epoll modify event failed][%d]: %s", errno, strerror(errno));
            return false;
        }

        log(NORMAL, "epoll modify event successed");
        return true;
    }

    bool mod(int fd, uint32_t events)
    {
        epoll_event ev;
        ev.data.fd = fd;
        ev.events = events;
        if (-1 == epoll_ctl(_epfd, EPOLL_CTL_MOD, fd, &ev))
        {
            log(ERROR, "[epoll modify event failed][%d]: %s", errno, strerror(errno));
            return false;
        }

        log(NORMAL, "epoll modify event successed");
        return true;
    }

    /////////////////////////////   wait  默认为阻塞
    int wait(epoll_event *events, int cap, int ms = BLOCK)
    {
        int n = epoll_wait(_epfd, events, cap, ms);
        if (-1 == n)
            log(ERROR, "[epoll wait error][%d]: %s", errno, strerror(errno));
        else if (0 == n)
            log(NORMAL, "epoll wait timeout...");
        else
            log(NORMAL, "epoll time wait successed");

        return n;
    }

    // int block_wait(epoll_event *events, int cap)
    // {
    //     int n = epoll_wait(_epfd, events, cap, -1);
    //     if (-1 == n)
    //     {
    //         log(ERROR, "[epoll wait error][%d]: %s", errno, strerror(errno));
    //         return -1;
    //     }

    //     log(NORMAL, "epoll block wait block successed");
    //     return n;
    // }

    // int nonblock_wait(epoll_event *events, int cap)
    // {
    //     int n = epoll_wait(_epfd, events, cap, 0);
    //     if (-1 == n)
    //     {
    //         log(ERROR, "[epoll wait error][%d]: %s", errno, strerror(errno));
    //         return -1;
    //     }

    //     log(NORMAL, "epoll nonblock wait successed");
    //     return n;
    // }

    // int time_wait(epoll_event *events, int cap, int ms)
    // {
    //     int n = epoll_wait(_epfd, events, cap, ms);
    //     if (-1 == n)
    //         log(ERROR, "[epoll wait error][%d]: %s", errno, strerror(errno));
    //     else if (0 == n)
    //         log(NORMAL, "epoll wait timeout...");
    //     else
    //         log(NORMAL, "epoll time wait successed");

    //     return n;
    // }

private:
    int _epfd;
};
