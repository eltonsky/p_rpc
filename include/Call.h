#ifndef CALL_H
#define CALL_H

#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include "Writable.h"
#include <string>
#include "IntWritable.h"

using namespace std;
using boost::asio::ip::tcp;

namespace Server{

    const int recheck_interval = 500;

    class Call
    {
        public:
            Call();
            Call(shared_ptr<tcp::socket> sock,int);
            virtual ~Call();
            bool read();
            bool write();

            string toString();

            inline const string getClass() const {return _class;}
            inline const string getMethod() const {return _method;}
            inline const vector<Writable>& getParams() const {return _params;}
            void setValue(shared_ptr<Writable> v) {
                _value = v;
            }
            inline const Writable* getValue() const {return _value.get();}
            inline const tcp::socket* getSock() const {return _sock.get();}

        protected:
        private:

            shared_ptr<tcp::socket> _sock;
            int _call_id;
            string _class;
            string _method;
            vector<Writable> _params;
            shared_ptr<Writable> _value;
    };

}

#endif // CALL_H
