#ifndef CLIENT_H
#define CLIENT_H

#include "Writable.h"
#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <cassert>
#include "Log.h"
#include "BlockQueue.h"
#include "Method.h"

using namespace std;
using boost::asio::ip::tcp;

class Client
{
    private:

        tcp::endpoint& _server_ep;
        const int _max_client_calls = 100;
        const long _call_wait_time = 500;

        boost::thread _t_recv_respond;
        boost::asio::io_service _io_service;

        bool _should_stop;

        std::mutex _mutex_client;
        std::condition_variable _cond_client;

    public:
        class Call {
            public:
                Call(){}

                Call(const Call& _c) : _done(false) {
                    _param = _c.getParam();
                    _valueClass = _c.getValueClass();
                }

                Call(shared_ptr<Writable> p, string value_class)
                : _valueClass(value_class), _param(p), _done(false) {}


                Call operator=(const Call& call) {
                    return Call(call);
                }

                virtual ~Call() {}

                void wait(long call_wait_time) {
                    std::unique_lock<std::mutex> ulock(_mutex);

                    while(!_done) {
                        if(_cond.wait_for(ulock, chrono::milliseconds(call_wait_time),
                            [this] { return _done; })) {
                            break;
                        }
                    }
                }


                void setValue() {

                    _value = Method::getNewInstance(_valueClass);

                    _value->readFields(_sock.get());

                    Log::write(DEBUG, "setValue : _value is %s\n", _value->toString().c_str());

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

                        Log::write(INFO, "call connected to %s : %d", ep.address().to_string().c_str(), ep.port());

                    } catch(...) {
                        Log::write(ERROR, "Failed to connect\n");
                        return false;
                    }

                    return true;
                }

                bool getDone() const {return _done;}
                tcp::socket* getConnection() const {return _sock.get();}
                shared_ptr<Writable> getValue() const {return _value;}
                shared_ptr<Writable> getParam() const {return _param;}
                string getValueClass() const {return _valueClass;}

            private:

                string _valueClass;
                int _id;
                shared_ptr<Writable> _param;
                shared_ptr<Writable> _value;
                bool _done;
                std::mutex _mutex;
                std::condition_variable _cond;
                shared_ptr<tcp::socket> _sock;
        };

        Client(tcp::endpoint ep);

        void sendCall(Call* call);
        bool waitForWork();
        void recvRespond();

        shared_ptr<Writable> call(shared_ptr<Call>);

        void start();

        void stop();

        virtual ~Client();


        private:

            BlockQueue<shared_ptr<Call>> _bq_client_calls;
};

#endif // CLIENT_H
