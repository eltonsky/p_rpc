#include "Call.h"

namespace Server {

    Call::Call()
    {
        //ctor
    }

    Call::Call(shared_ptr<tcp::socket> sock, int call_id) : _sock(sock), _call_id(call_id) {
    }


    bool Call::read() {
        if(_sock.get() == NULL)
            return false;

        try{
            _class = Writable::readString(_sock.get());

            _method = Writable::readString(_sock.get());

            //params
            size_t size = -1;
            size_t l = boost::asio::read(*(_sock.get()),
                boost::asio::buffer(&size, sizeof(size)));

            if(l <= 0) {
                Log::write(ERROR, "Fail to read size of params, class %s, method %s\n",
                       _class.c_str(), _method.c_str());
                return -1;
            }

            _params.reserve(size);

            for(size_t i =0; i < size; i++) {
                string param_class = Writable::readString(_sock.get());

                _params.push_back(Method::getNewInstance(param_class));

                _params[i]->readFields(_sock.get());
            }

        } catch(exception& e) {
            Log::write(ERROR, "Exception when read a call <%s> : %s\n", toString().c_str(), e.what());
            return false;
        }

        return true;
    }


    bool Call::write() {

        try{
            //just write value back for now.
            _value->write(_sock.get());
        }
         catch(exception& e) {
            Log::write(ERROR, "Exception when write a call result <%s : %s> : %s \n",
                       toString().c_str(), _value.get()->toString().c_str(), e.what());
            return false;
        }

        return true;
    }


    string Call::toString() {
        stringstream ss;
        ss <<"call_id "<<_call_id<<", class "<<_class<<" , method "<<_method;
        return ss.str();
    }


    string Call::print() {
        stringstream ss;
        ss <<"call_id "<<_call_id<<", class "<<_class<<" , method "<<_method;

        for(size_t i=0; i < _params.size(); i++) {
            ss<<"\n param "<<i<<" : " << _params[i]->toString();
        }

        return ss.str();
    }


    Call::~Call()
    {
        //dtor
    }

}
