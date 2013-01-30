#ifndef LISTENER_H
#define LISTENER_H

#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/lexical_cast.hpp>
#include <sys/syscall.h>
#include <vector>
#include <queue>
#include <memory>
#include <semaphore.h>
#include <csignal>
#include "BlockQueue.h"

using boost::asio::ip::tcp;

namespace Server {

class Listener
{
    public:
        boost::asio::io_service _io_service_listener;
        boost::asio::io_service _io_service_disp;

        BlockQueue<std::shared_ptr<tcp::socket>> _bq_acpt_sock;

        Listener(int port);

        virtual ~Listener();

        void start();

    private:

        int _port;
        tcp::acceptor _acceptor;
        std::shared_ptr<tcp::socket> _curr_sock;
};


class Dispatcher {

    std::shared_ptr<tcp::socket> _d_sock;
    Listener& _listener;

public:
    Dispatcher(Listener& listener) : _listener(listener){
    }

    virtual ~Dispatcher(){}


    void handle_read(std::shared_ptr<tcp::socket> sock, const boost::system::error_code& error,
      size_t bytes_transferred)
    {

        cout<<"handle_read: error " << error<<endl;

        _d_sock = _listener._bq_acpt_sock.pop();

        _d_sock.get()->async_read_some(boost::asio::null_buffers(),
        boost::bind(&Dispatcher::handle_read, this, _d_sock,
          boost::asio::placeholders::error,
          boost::asio::placeholders::bytes_transferred));
    }

    void start() {

        printf("Dispatcher started. <thread id : %ld>, <pid : %d> \n", (long int)syscall(SYS_gettid), getpid());

        _d_sock = _listener._bq_acpt_sock.pop();

        _d_sock.get()->async_read_some(boost::asio::null_buffers(),
        boost::bind(&Dispatcher::handle_read, this, _d_sock,
          boost::asio::placeholders::error,
          boost::asio::placeholders::bytes_transferred));

        _listener._io_service_disp.run();

    }

};


class Reader {

public:

    Reader(){}
    virtual ~Reader(){}

    void start() {
        printf("Reader started. <thread id : %ld>, <pid : %d> \n", (long int)syscall(SYS_gettid), getpid());
    }
};


}

#endif // LISTENER_H
