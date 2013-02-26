#ifndef CLIENT_H
#define CLIENT_H

#include "Writable.h"
#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <cassert>
#include "Log.h"
#include "BlockQueue.h"

using namespace std;
using boost::asio::ip::tcp;

class Client
{
    private:

        class Call {
            public:

                Call(const Call& _c) : _done(false) {
                    _param = _c.getParam();
                }

                Call(Writable p)
                : _param(p), _done(false) {}

                virtual ~Call() {}

                void wait(long call_wait_time) {
                    std::unique_lock<std::mutex> ulock(_mutex);

                    while(true) {
                        _cond.wait_for(ulock, chrono::milliseconds(call_wait_time),
                            [this] { return _done; });
                    }
                }

                void setValue(Writable res) {
                    _value = res;
                    _done = true;
                    _cond.notify_all();
                }

                bool connect(tcp::endpoint ep) {
                    try{
                        boost::asio::io_service io_service;
                        tcp::resolver resolver(io_service);
                        tcp::resolver::query query(tcp::v4(), ep.address().to_string(), std::to_string(ep.port()));
                        tcp::resolver::iterator iterator = resolver.resolve(query);

                        shared_ptr<tcp::socket> s = shared_ptr<tcp::socket>(new tcp::socket(io_service));
                        s.get()->connect(*iterator);

                        _sock = s;

                    } catch(...) {
                        Log::write(ERROR, "Failed to connect\n");
                        return false;
                    }

                    return true;
                }

                bool getDone() const {return _done;}
                tcp::socket* getConnection() const {return _sock.get();}
                Writable getValue() const {return _value;}
                Writable getParam() const {return _param;}

            private:
                int _id;
                Writable _param;
                Writable _value;
                bool _done;
                std::mutex _mutex;
                std::condition_variable _cond;
                shared_ptr<tcp::socket> _sock;
        };


        string _valueClass;
        tcp::endpoint _server_ep;
        const int _max_client_calls = 100;
        const long _call_wait_time = 500;
        BlockQueue<Call> _bq_client_calls;

        boost::thread _t_recv_respond;
        boost::asio::io_service _io_service;

        bool _should_stop;

        std::mutex _mutex_client;
        std::condition_variable _cond_client;

    public:
        Client();
        Client(string valueClass, tcp::endpoint ep);

        void sendCall(Call& call);
        bool waitForWork();
        void recvRespond();

        Writable call(Call call);

        void start();

        void stop();

        virtual ~Client();
};

#endif // CLIENT_H
