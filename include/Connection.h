#ifndef CONNECTION_H
#define CONNECTION_H

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

namespace Server {
    class Call;

    extern int _last_connection_index;
    extern std::mutex _mutex_conns;
    extern std::condition_variable _cond_conns;

    class Connection{

        shared_ptr<tcp::socket> _sock;
        shared_ptr<tcp::endpoint> _ep;
        std::mutex _mutex;
        const int _max_respond_size = 100;

    public:
        BlockQueue<shared_ptr<Call>> respond_queue;
        int index;

        Connection(shared_ptr<tcp::socket> sock,
                   shared_ptr<tcp::endpoint> ep, int i);

        int processResponse(shared_ptr<Call>);

        void close();

        string toString();

        inline shared_ptr<tcp::socket> getSock() const{
            return _sock;
        }

        inline shared_ptr<tcp::endpoint> getEndpoint() const {
            return _ep;
        }

    };

    // delcare map after definition of connection map.
    extern map<tcp::endpoint, shared_ptr<Connection>> _connections;

}

#endif // CONNECTION_H
