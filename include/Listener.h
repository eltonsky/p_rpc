#ifndef LISTENER_H
#define LISTENER_H

#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <sys/syscall.h>
#include <vector>
#include <queue>
#include <memory>
#include <semaphore.h>
#include <csignal>
#include "BlockQueue.h"
#include "Call.h"
#include "Handler.h"

using boost::asio::ip::tcp;

namespace Server {

class Listener
{
    public:
        bool _should_stop = false;
        static const short num_readers = 2;
        const short max_reader_queue_size = 10;
        boost::asio::io_service _io_service_listener;
        boost::asio::io_service _io_service_disp;
        BlockQueue<std::shared_ptr<tcp::socket>> _bq_acpt_sock;
        int max_accept_conns;

        Listener(int port);

        virtual ~Listener();

        void run();

        void start();

        void stop();

        void join();

        void handle_read(shared_ptr<tcp::socket> sock, const boost::system::error_code& error,
              size_t bytes_transferred);

        void handle_accept(shared_ptr<tcp::socket>& sock,
            const boost::system::error_code& error);

        bool addToReader(shared_ptr<tcp::socket> sock);

    private:

        int _port;
        int _last_reader_index;

        tcp::acceptor _acceptor;
        std::shared_ptr<tcp::socket> _curr_sock;

        boost::thread _t_listener;

        void _do_accept();

        //Reader
        class Reader {

            BlockQueue<shared_ptr<tcp::socket>> _bq_sock;
            boost::thread _t_reader;
            const short _reader_index;

            void _do_read(shared_ptr<tcp::socket> sock) {

                Log::write(INFO, "_do_read from reader %d\n", _reader_index);

                try{
                    //try to read a full call object
                    Call* call = new Call(sock, ++last_call_id);

                    if(!call->read()){
                        throw "Failed to read call in Reader";
                    }

                    Log::write(DEBUG, "Reader %d get call %s\n", _reader_index, call->toString().c_str());

                    shared_ptr<Call> s_call(call);

                    if(!_bq_call.try_push(s_call)) {
                        throw "FATAL: can not insert call into _bq_call. is it full !?";
                    }

                    Log::write(INFO, "_bq_call.size() %d\n", _bq_call.size());

                } catch (exception& e) {
                    Log::write(ERROR, e.what());
                }
            }

        public:

            bool _should_stop = false;

            Reader(short index, short max_q_size) :
                _bq_sock(max_q_size),
                _reader_index(index){
            }

            virtual ~Reader(){}

            void join() {
                _t_reader.join();
            }

            void run() {
                _t_reader = boost::thread(boost::bind(&Server::Listener::Reader::start, this));
            }

            void start() {
                Log::write(INFO, "Reader %d started. <thread id : %ld>, <pid : %d> \n",
                       _reader_index, (long int)syscall(SYS_gettid), getpid());

                while(!_should_stop) {

                    _do_read(_bq_sock.pop());
                }
            }

            bool add(shared_ptr<tcp::socket> sock) {
                return _bq_sock.try_push(sock);
            }

            void waitToFinish() {
                while(_bq_sock.size() != 0) {
                    this_thread::sleep_for(chrono::milliseconds(recheck_interval));
                }

                Log::write(INFO, "Reader %d finished\n", _reader_index);

                // stop producing any more calls
                _should_stop = true;

                _t_reader.interrupt();
            }

        };

        shared_ptr<Reader> _readers[num_readers];
};


}

#endif // LISTENER_H
