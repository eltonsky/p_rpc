#ifndef WRITABLE_H
#define WRITABLE_H

#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/thread.hpp>

using boost::asio::ip::tcp;
using namespace std;

class Writable
{
    public:
        Writable();
        virtual ~Writable();
        virtual void readFields(tcp::socket * sock);
        virtual void write(tcp::socket * sock);

        static int writeString(tcp::socket*sock, string str);
        static string readString(tcp::socket * sock);

    protected:
    private:
};

#endif // WRITABLE_H
