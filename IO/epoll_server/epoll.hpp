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

class epoll
{
    const static int _size = 128;

public:
    static int create()
    {
        int n = epoll_create(_size);
        if (-1 == n)
        {
            log(FATAL, "[epoll create failed][%d]: %s", errno, strerror(errno));
            exit(EPOLL_CREATE_ERR);
        }

        log(NORMAL, "epoll create successed");
        return n;
    }

    static bool add(int epfd, int fd, epoll_event *event)
    {
        if (-1 == epoll_ctl(epfd, EPOLL_CTL_ADD, fd, event))
        {
            log(ERROR, "[epoll add fd failed][%d]: %s", errno, strerror(errno));
            return false;
        }

        log(NORMAL, "epoll add fd successed");
        return true;
    }

    static bool del(int epfd, int fd)
    {
        // epoll模型中的红黑树是以fd为key值的，当删除红黑树节点时，需要关心节点内容是什么吗？
        // 显然是不需要的
        // 所以这里的epoll_event* 是被忽略的，可以直接传空指针
        if (-1 == epoll_ctl(epfd, EPOLL_CTL_DEL, fd, nullptr))
        {
            log(ERROR, "[epoll delete fd failed][%d]: %s", errno, strerror(errno));
            return false;
        }

        log(NORMAL, "epoll delete fd successed");
        return true;
    }

    static bool mod(int epfd, int fd, epoll_event *event)
    {
        if (-1 == epoll_ctl(epfd, EPOLL_CTL_MOD, fd, event))
        {
            log(ERROR, "[epoll modify event failed][%d]: %s", errno, strerror(errno));
            return false;
        }

        log(NORMAL, "epoll modify event successed");
        return true;
    }

    static int block_wait(int epfd, epoll_event *events, int cap)
    {
        int n = epoll_wait(epfd, events, cap, -1);
        if (-1 == n)
        {
            log(ERROR, "[epoll wait error][%d]: %s", errno, strerror(errno));
            return -1;
        }

        log(NORMAL, "epoll block wait block successed");
        return n;
    }

    static int nonblock_wait(int epfd, epoll_event *events, int cap)
    {
        int n = epoll_wait(epfd, events, cap, 0);
        if (-1 == n)
        {
            log(ERROR, "[epoll wait error][%d]: %s", errno, strerror(errno));
            return -1;
        }

        log(NORMAL, "epoll nonblock wait successed");
        return n;
    }

    static int time_wait(int epfd, epoll_event *events, int cap, int ms)
    {
        int n = epoll_wait(epfd, events, cap, ms);
        if (-1 == n)
            log(ERROR, "[epoll wait error][%d]: %s", errno, strerror(errno));
        else if (0 == n)
            log(NORMAL, "epoll wait timeout...");
        else
            log(NORMAL, "epoll time wait successed");

        return n;
    }
};
