#pragma once

#include <string>
#include <unordered_map>
#include <functional>

// class server_reactor;
// struct connection;

// using connptr = connection *;
// using conf_t = std::function<void(connptr)>;

// struct connection
// {
// public:
//     connection(int sock, conf_t recv, conf_t write, conf_t except) : _sock(sock), _recver(recv), _excepter(except), _psvr(nullptr) {}

// public:
//     int _sock; // 连接对应的文件描述符/套接字

//     std::string _recvbuf;   // 读取数据到该缓冲区
//     std::string _writebuf;  // 写数据到该缓冲区
//     std::string _exceptbuf; // 异常事件写到该缓冲区

//     conf_t _recver;   // 读方法
//     conf_t _writer;   // 写方法
//     conf_t _excepter; // 异常处理方法

//     server_reactor *_psvr; // server回指指针
// };

// class conn_manager
// {
// public:
//     conn_manager() {}
//     ~conn_manager() {}

// public:
//     connptr newconn(int sock, conf_t recv, conf_t write, conf_t except)
//     {
//         connptr pc = new connection(sock, recv, write, except);

//         _conns.insert(std::make_pair(sock, pc));
//         return pc;
//     }

//     void deleconn(int sock)
//     {
//         delete _conns[sock];
//         _conns[sock] = nullptr;
//         _conns.erase(sock);
//     }

//     connptr getconnptr(int sock)
//     {
//         return _conns[sock];
//     }

// private:
//     std::unordered_map<int, connptr> _conns;
// };








// template <class PTR>
// struct connection;

// template <class T>
// class conn_manager;

// template <class PTR>
// using conf_t = std::function<void(connection<PTR> *)>;

// template <class T>
// using connptr = connection<T *> *;

// template <class PTR>
// struct connection
// {
// public:
//     connection(int sock, conf_t recv, conf_t write, conf_t except) : _sock(sock), _recver(recv), _excepter(except) {}

// public:
//     int _sock; // 连接对应的文件描述符/套接字

//     std::string _recvbuf;   // 读取数据到该缓冲区
//     std::string _writebuf;  // 写数据到该缓冲区
//     std::string _exceptbuf; // 异常事件写到该缓冲区

//     conf_t _recver;   // 读方法
//     conf_t _writer;   // 写方法
//     conf_t _excepter; // 异常处理方法

//     PTR _psvr; // server回指指针
// };

// template <class T>
// class conn_manager
// {
// public:
//     conn_manager() {}
//     ~conn_manager() {}

// public:
//     connptr newconn(int sock, conf_t recv, conf_t write, conf_t except)
//     {
//         connptr pc = new connection<T *>(sock, recv, write, except);

//         _conns.insert(std::make_pair(sock, pc));
//         return pc;
//     }

//     void deleconn(int sock)
//     {
//         delete _conns[sock];
//         _conns[sock] = nullptr;
//         _conns.erase(sock);
//     }

//     connptr getconnptr(int sock)
//     {
//         return _conns[sock];
//     }

// private:
//     std::unordered_map<int, connptr> _conns;
// };
