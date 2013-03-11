#include "Writable.h"

Writable::Writable()
{
    //ctor
}

Writable::~Writable()
{
    //dtor
}

int Writable::readFields(tcp::socket * sock) {return 0;}

int Writable::write(tcp::socket * sock) {return 0;}

string Writable::toString() {return NULL;}

string Writable::printToString() {return NULL;}


int Writable::writeString(tcp::socket*sock, string str) {
    int length = str.size();

    if(sock == NULL || length == 0)
        return 0;

    size_t l = boost::asio::write(*sock, boost::asio::buffer((const char*)&length, sizeof(length)));

    Log::write(DEBUG, "writeString : length of length %d\n", l);

    size_t send_length =
        boost::asio::write(*sock, boost::asio::buffer(str.c_str(), str.size()));

    Log::write(DEBUG, "writeString : write %s length %d\n", str.c_str(), send_length);

    return send_length;
}


string Writable::readString(tcp::socket * sock){
    if(sock == NULL)
        return NULL;

    int length = -1;

    size_t l = boost::asio::read(*sock,
            boost::asio::buffer(&length, sizeof(length)));

    Log::write(DEBUG, "readString : length of length %d\n", l);

    if(length > 0) {
        char char_str[length];

        size_t reply_length = boost::asio::read(*sock,
            boost::asio::buffer(char_str, length));

        Log::write(DEBUG, "readString : %s, length %d\n", char_str, reply_length);

        if(reply_length > 0) {
            return string(char_str);
        }
    }

    return NULL;
}


string Writable::getClass() { return "Writable";}





