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
        class Connection;

        // Call
        class Call {
        public:
            Call(){}

            Call(const Call& _c) :
                 _valueClass(_c.getValueClass()),
                 _id(_c.getId()), _param(_c.getParam()),
                 _done(false) {}



            Call(shared_ptr<Writable> p, string value_class)
                : _valueClass(value_class), _param(p),
                 _done(false){}


            Call(shared_ptr<Writable> p, string value_class, int id)
                : _valueClass(value_class),
                  _id(id), _param(p), _done(false) {}


            Call operator=(const Call& call) {
                return Call(call);
            }

            ~Call() {
            }

            void wait(long call_wait_time) {
                std::unique_lock<std::mutex> ulock(_mutex);

                while(!_done) {
                    if(_cond.wait_for(ulock, chrono::milliseconds(call_wait_time),
                        [this] { return _done; })) {
                        break;
                    }
                }
            }


            void setValue(shared_ptr<Writable> val) {
                _value = val;

                Log::write(DEBUG, "setValue : value is %s\n", val->toString().c_str());

                _done = true;
                _cond.notify_all();
            }


            bool write() {
                tcp::socket* sock = _connection->getSock();

                size_t l = boost::asio::write(*sock, boost::asio::buffer((const char*)&_id, sizeof(_id)));

                if(l <= 0) {
                    Log::write(ERROR, "Failed to send call.id %d\n.", _id);
                    return false;
                }

                if (_param->write(sock) < 0) {
                    Log::write(ERROR, "Failed to send call - id:%d , param: %s\n", _id, _param->toString().c_str());
                    return false;
                }

                return true;
            }


            inline const bool getDone() const {return _done;}
            inline Connection* getConnection() const {return _connection.get();}
            inline const shared_ptr<Writable> getValue() const {return _value;}
            inline const shared_ptr<Writable> getParam() const {return _param;}
            inline const string getValueClass() const {return _valueClass;}
            inline const int getId() const {return _id;}

            inline void setId(int id) {
                _id = id;
            }

            inline void setConnection(shared_ptr<Connection> conn) {
                _connection = conn;
            }

        private:

            string _valueClass;
            int _id;
            shared_ptr<Connection> _connection;
            shared_ptr<Writable> _param;
            shared_ptr<Writable> _value;
            bool _done;
            std::mutex _mutex;
            std::condition_variable _cond;
            boost::asio::io_service _io_service;
        };


        //Connection
        class Connection {
            boost::thread _t_recv_respond;
            boost::asio::io_service _io_service;
            tcp::socket* _sock;
            const int _max_conn_calls = 100;
            shared_ptr<tcp::endpoint> _ep;

        public:
            std::mutex _mutex_conn;
            std::condition_variable _cond_conn;

            int last_call_index = 0; //only used in Client::getConnection()
            BlockQueue<shared_ptr<Call>> bq_conn_calls;

            Connection(shared_ptr<tcp::endpoint>);
            ~Connection();

            bool waitForWork(); // not used.

            void recvRespond(shared_ptr<Call>);

            void recvStart();

            inline string toString() {
                stringstream ss;
                ss <<_ep->address().to_string();
                ss <<":";
                ss<<_ep->port();
                return ss.str();
            }

            inline tcp::socket* getSock() const{
                return _sock;
            }
        };


        int _last_connection_index = 0;
        const int _max_connection_num = 32768;
        map<shared_ptr<tcp::endpoint>,shared_ptr<Connection>> _connections;

        shared_ptr<tcp::endpoint> _server_ep;
        const int _max_client_calls = 100;

        static const long _call_wait_time;
        static bool _should_stop;

        std::mutex _mutex_client;
        std::condition_variable _cond_client;

    public:

        Client(shared_ptr<tcp::endpoint> ep);
        Client();

        void sendCall(Call* call);

        shared_ptr<Writable> call(shared_ptr<Writable> param,
                                  string value_class,
                                  shared_ptr<tcp::endpoint>);

        void start();

        void stop();

        shared_ptr<Connection> getConnection(shared_ptr<tcp::endpoint>,
                                             shared_ptr<Call>);

        ~Client();
};

#endif // CLIENT_H




