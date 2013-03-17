#ifndef TEST2_H
#define TEST2_H

#include <string>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <thread>
#include <mutex>
#include "BlockQueue.h"
#include <sys/syscall.h>
#include <boost/asio.hpp>
#include <chrono>
#include <condition_variable>
#include "unistd.h"
#include "stdlib.h"
#include "Listener.h"
#include <signal.h>
#include "Utils.h"
#include "Server.h"
#include "Client.h"
#include "Log.h"
#include "MethodWritable.h"
#include "RPC.h"
#include "Invocation.h"

class Test2
{
    public:

        class DataNodeProtocol {
            public:

            DataNodeProtocol(){}

            DataNodeProtocol(string server_host, int port){

                boost::system::error_code error;
                boost::asio::ip::address_v4 targetIP;
                targetIP = boost::asio::ip::address_v4::from_string(server_host.c_str(), error);

                _ep = make_shared<tcp::endpoint>();
                _ep->address(targetIP);
                _ep->port(port);
            }

            virtual ~DataNodeProtocol(){}

            virtual shared_ptr<Writable> create(shared_ptr<IntWritable> Arg0,
                                                shared_ptr<IntWritable> Arg1,
                                                shared_ptr<IntWritable> Arg2,
                                                shared_ptr<IntWritable> Arg3) {

                return RPC::invoke("standard", _ep, "IntWritable", "NameNode", "create", Arg0, Arg1, Arg2, Arg3);
            }

            //...

            private:
                shared_ptr<tcp::endpoint> _ep;
        };


        class NameNode : DataNodeProtocol{
            public:

            NameNode(){}
            ~NameNode(){}

            virtual shared_ptr<Writable> create(shared_ptr<IntWritable> Arg0,
                                                shared_ptr<IntWritable> Arg1,
                                                shared_ptr<IntWritable> Arg2,
                                                shared_ptr<IntWritable> Arg3) {
                int res = Arg0->get() + Arg1->get() + Arg2->get() + Arg3->get();

                return make_shared<IntWritable>(res);
            }

            //...

        };


        Test2();
        ~Test2();

        void test21(string server_host, int port) ;

    protected:
    private:

};

#endif // TEST2_H
