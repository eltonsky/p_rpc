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
        bool _should_stop = false;
        int max_accept_conns;

        Listener(int port);

        virtual ~Listener();

        void run();

        void start();

        void stop();

        void join();

    private:

        int _port;

        tcp::acceptor _acceptor;
        std::shared_ptr<tcp::socket> _curr_sock;

        boost::thread _t_listener;
        boost::thread _t_disp;
        vector<boost::thread> _t_readers;


        class Dispatcher {

            std::shared_ptr<tcp::socket> _d_sock;
            Listener& _parent_l;

        public:

            Dispatcher(Listener& l) : _parent_l(l) {}

            virtual ~Dispatcher(){}

            void handle_read(std::shared_ptr<tcp::socket> sock, const boost::system::error_code& error,
              size_t bytes_transferred)
            {

                cout<<"handle_read: error " << error<<endl;

                if(!_parent_l._should_stop) {

                    _d_sock = _parent_l._bq_acpt_sock.pop();

                    _d_sock.get()->async_read_some(boost::asio::null_buffers(),
                    boost::bind(&Dispatcher::handle_read, this, _d_sock,
                      boost::asio::placeholders::error,
                      boost::asio::placeholders::bytes_transferred));
                }
            }

            void start() {

                printf("Dispatcher started. <thread id : %ld>, <pid : %d> \n", (long int)syscall(SYS_gettid), getpid());

                _d_sock = _parent_l._bq_acpt_sock.pop();

                _d_sock.get()->async_read_some(boost::asio::null_buffers(),
                boost::bind(&Dispatcher::handle_read, this, _d_sock,
                  boost::asio::placeholders::error,
                  boost::asio::placeholders::bytes_transferred));

                _parent_l._io_service_disp.run();

            }

        };


        class Reader {

        public:

            Reader(){}
            virtual ~Reader(){}

            void join(){}

            void start() {
                printf("Reader started. <thread id : %ld>, <pid : %d> \n", (long int)syscall(SYS_gettid), getpid());
            }
        };


        Dispatcher _disp;
        vector<shared_ptr<Reader>> _readers;
};


}

#endif // LISTENER_H
