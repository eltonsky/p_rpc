#ifndef CALL_H
#define CALL_H

#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include "Writable.h"
#include <string>

using namespace std;
using boost::asio::ip::tcp;

namespace Server{

    const int recheck_interval = 500;

    class Call
    {
        public:
            Call();
            Call(tcp::socket* sock,int);
            virtual ~Call();
            bool read();
            bool write();

            void print();

        protected:
        private:

            int _call_id;
            tcp::socket* _sock;
            string _class;
            string _method;
            vector<Writable> params;
            Writable value;
    };

}

#endif // CALL_H
