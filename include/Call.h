#ifndef CALL_H
#define CALL_H

#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include "Writable.h"
#include "Method.h"
#include <string>
#include "IntWritable.h"
#include "BlockQueue.h"

using namespace std;
using boost::asio::ip::tcp;

namespace Server{
    class Connection;

    static int _last_connection_index = 0;
    const int recheck_interval = 100;
    map<tcp::endpoint, shared_ptr<Connection>> _connections;

    class Call
    {
        public:
            Call();
            Call(shared_ptr<tcp::socket> sock);
            ~Call();
            bool read();
            bool write();

            string toString();

            // for test
            string print();

            inline const string getClass() const {return _class;}
            inline const string getMethod() const {return _method;}
            inline const vector<shared_ptr<Writable>> getParams() const {return _params;}
            void setValue(shared_ptr<Writable> v) {
                _value = v;
            }
            inline const Writable* getValue() const {return _value.get();}
            inline const tcp::socket* getSock() const {return _connection->getSock()->get();}
            inline void setConnection(shared_ptr<Connection> conn) {_connection = conn;}
            inline void setId(int id) {_call_id = id;}

        protected:
        private:

            shared_ptr<Connection> _connection;
            int _call_id;
            string _class;
            string _method;
            vector<shared_ptr<Writable>> _params;
            shared_ptr<Writable> _value;
    };


    class Connection{

        shared_ptr<tcp::socket> _sock;
        shared_ptr<tcp::endpoint> _ep;

    public:
        BlockQueue<shared_ptr<Call>> respond_queue;
        int index;

        Connection(tcp::socket sock, tcp::endpoint ep, int i);

        void close();

        string toString();
        inline tcp::socket* getSock() const{
            return _sock.get();
        }

        inline tcp::endpoint* getEndpoing() const {
            return _ep.get();
        }

    };




}

#endif // CALL_H
