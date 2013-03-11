#include "IntWritable.h"

IntWritable::IntWritable(){}

IntWritable::IntWritable(int v) : _value(v)
{
    //ctor
}


int IntWritable::readFields(tcp::socket * sock) {
    if(sock == NULL)
        return -1;

    size_t length;
    size_t l = boost::asio::read(*sock,
            boost::asio::buffer(&length, sizeof(length)));

    char buf[length];

    l = boost::asio::read(*sock,
            boost::asio::buffer(buf, length));

    if(l != length) {
        Log::write(ERROR,
                   "IntWritable::readFields: expected length %d, read length %d\n",
                   length, l);
        return -1;
    }

    //convert
    _value = atoi(buf);

    return l;
}


int IntWritable::write(tcp::socket * sock){

    if(sock == NULL)
        return -1;

    size_t l = boost::asio::write(*sock, boost::asio::buffer((const char*)&_value, sizeof(_value)));

    return l;
}


string IntWritable::printToString() {
    return std::to_string(_value);
}


string IntWritable::toString() {
    stringbuf sb;
    sb.sputn((const char*)&_value, sizeof _value);
    return sb.str();
}


string IntWritable::getClass(){
    return "IntWritable";
}


IntWritable::~IntWritable()
{
    //dtor
}
