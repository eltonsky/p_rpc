#ifndef WRITABLE_H
#define WRITABLE_H

#include <cstdlib>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/thread.hpp>

using boost::asio::ip::tcp;

class Writable
{
    public:
        Writable();
        virtual ~Writable();
        virtual void readFields(tcp::socket* sock);
        virtual void write(tcp::socket* sock);

    protected:
    private:
};

#endif // WRITABLE_H
