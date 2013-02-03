#include "Writable.h"

Writable::Writable()
{
    //ctor
}

Writable::~Writable()
{
    //dtor
}

void Writable::readFields(tcp::socket * sock) {}

void Writable::write(tcp::socket * sock){}

int Writable::writeString(tcp::socket*sock, string str) {
    int length = str.size();

cout<<"writeString : length " <<length<<endl;

    if(sock == NULL || length == 0)
        return 0;

    size_t l = boost::asio::write(*sock, boost::asio::buffer((const char*)&length, sizeof(length)));

cout<<"writeString : write length " <<l<<endl;

    size_t send_length =
        boost::asio::write(*sock, boost::asio::buffer(str.c_str(), str.size()));

cout<<"writeString : send  length " <<send_length<<endl;

    return send_length;
}

string Writable::readString(tcp::socket * sock){
    if(sock == NULL)
        return NULL;

    int length = -1;

    size_t l = boost::asio::read(*sock,
            boost::asio::buffer(&length, sizeof(length)));

cout<<"readString : l " <<l<<" , length "<<length<<endl;

    if(length > 0) {
        char char_str[length];

        size_t reply_length = boost::asio::read(*sock,
            boost::asio::buffer(char_str, length));

cout<<"readString : reply_length " <<reply_length<<" , char_str "<<char_str<<endl;


        if(reply_length > 0)
            return string(char_str);
    }

    return NULL;
}
