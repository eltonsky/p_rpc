#include "MethodWritable.h"

MethodWritable::MethodWritable()
{
    //ctor
}


MethodWritable::MethodWritable(string c, string m, vector<Writable>& p)
    : _class_name(c), _method_name(m), _params(p) {}


int MethodWritable::readFields(tcp::socket * sock) {

    if(sock == NULL)
        return -1;

    try{
        //class
        _class_name = Writable::readString(sock);

        //method
        _method_name = Writable::readString(sock);

        //params
        size_t size = -1;
        size_t l = boost::asio::read(*sock,
            boost::asio::buffer(&size, sizeof(size)));

        if(l <= 0) {
            Log::write(ERROR, "Fail to read size of params, class %s, method %s\n",
                   _class_name.c_str(), _method_name.c_str());
            return -1;
        }

        _params.reserve(size);
        for(size_t i =0; i < size; i++) {
            _params[i].readFields(sock);
        }
    } catch(...){
        Log::write(ERROR, "Fail to write MethodWritable, class %s, method %s\n",
                   _class_name.c_str(), _method_name.c_str());
        return -1;
    }

    return 0;
}


int MethodWritable::write(tcp::socket * sock){

    if(sock == NULL)
        return -1;

    try{
        //class
        Writable::writeString(sock, _class_name);

        //method
        Writable::writeString(sock, _method_name);

        //params
        size_t size = _params.size();
        size_t l = boost::asio::write(*sock, boost::asio::buffer((const char*)&size, sizeof(size)));

        if(l <= 0) {
            Log::write(ERROR, "Fail to write size of params, class %s, method %s\n",
                   _class_name.c_str(), _method_name.c_str());
            return -1;
        }

        for(size_t i =0; i < size; i++) {
            _params[i].write(sock);
        }

    } catch(...){
        Log::write(ERROR, "Fail to write MethodWritable, class %s, method %s\n",
                   _class_name.c_str(), _method_name.c_str());
        return -1;
    }

    return 0;
}


MethodWritable::~MethodWritable()
{
    //dtor
}



