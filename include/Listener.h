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

class Listener
{
    public:
        boost::asio::io_service& _io_service_listener;
        boost::asio::io_service& _io_service_disp;
        BlockQueue<std::shared_ptr<tcp::socket>> _bq_acpt_sock;

        Listener(boost::asio::io_service& io_service_l,
                 boost::asio::io_service& io_service_d,
                 int port);

        virtual ~Listener();
    protected:
    private:

        int _port;
        tcp::acceptor _acceptor;
        std::shared_ptr<tcp::socket> _curr_sock;

        class Dispatcher {

            std::shared_ptr<tcp::socket> _d_sock;
            Listener& _listener;

        public:
            Dispatcher(Listener& listener) : _listener(listener) {

            }

            virtual ~Dispatcher(){}


            void handle_read1(std::shared_ptr<tcp::socket> sock, const boost::system::error_code& error,
              size_t bytes_transferred)
            {
                _d_sock = _listener._bq_acpt_sock.pop();

                _d_sock.get()->async_read_some(boost::asio::null_buffers(),
                boost::bind(&Dispatcher::handle_read1, this, _d_sock,
                  boost::asio::placeholders::error,
                  boost::asio::placeholders::bytes_transferred));
            }

            void start() {

                cout<<"Dispatcher started .. "<<endl;
                printf("The ID of this of this thread is: %ld, pid : %d \n", (long int)syscall(SYS_gettid), getpid());

                _d_sock = _listener._bq_acpt_sock.pop();

                _d_sock.get()->async_read_some(boost::asio::null_buffers(),
                boost::bind(&Dispatcher::handle_read1, this, _d_sock,
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
                cout<<"Reader started .."<<endl;
                printf("The ID of this of this thread is: %ld, pid : %d \n", (long int)syscall(SYS_gettid), getpid());
            }
        };

};

#endif // LISTENER_H
