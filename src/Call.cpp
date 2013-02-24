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

//TODO:read param

        } catch(exception& e) {
            cout<<e.what()<<endl;
            return false;
        }

        return true;
    }

    bool Call::write() {

        try{
            //just write value back for now.
            _value.get()->write(_sock.get());
        }
         catch(exception& e) {
            cout<<e.what()<<endl;
            return false;
        }

        return true;
    }


    string Call::toString() {
        stringstream ss;
        ss <<"call_id "<<_call_id<<", class "<<_class<<" , method "<<_method;
        return ss.str();
    }


    Call::~Call()
    {
        //dtor
    }

}
