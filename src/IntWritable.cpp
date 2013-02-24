#include "IntWritable.h"

IntWritable::IntWritable(int v) : _value(v)
{
    //ctor
}


int IntWritable::readFields(tcp::socket * sock) {
    size_t l = boost::asio::read(*sock,
            boost::asio::buffer(&_value, sizeof(_value)));

    return l;
}


int IntWritable::write(tcp::socket * sock){

    if(sock == NULL)
        return 0;

    size_t l = boost::asio::write(*sock, boost::asio::buffer((const char*)&_value, sizeof(_value)));

    return l;
}


IntWritable::~IntWritable()
{
    //dtor
}
