#include "Call.h"

namespace Server {

    Call::Call()
    {
        //ctor
    }

    Call::Call(shared_ptr<Connection> conn) : _connection(conn) {
    }


    bool Call::read() {
        if(_connection.get() == NULL)
            return false;

        tcp::socket* sock = _connection->getSock()->get();

        try{
            size_t l = boost::asio::read(*(sock),
                boost::asio::buffer(&_curr_id, sizeof(_curr_id)));
             if(l <= 0) {
                Log::write(ERROR, "Fail to read call id\n");
                return -1;
            }

            _class = Writable::readString(sock);

            _method = Writable::readString(sock);

            //params
            size_t size = -1;
            l = boost::asio::read(*(sock),
                boost::asio::buffer(&size, sizeof(size)));

            if(l <= 0) {
                Log::write(ERROR, "Fail to read size of params, class %s, method %s\n",
                       _class.c_str(), _method.c_str());
                return -1;
            }

            _params.reserve(size);

            for(size_t i =0; i < size; i++) {
                string param_class = Writable::readString(sock);

                _params.push_back(Method::getNewInstance(param_class));

                _params[i]->readFields(sock);
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
