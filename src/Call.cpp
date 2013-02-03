#include "Call.h"

namespace Server {

    Call::Call()
    {
        //ctor
    }

    Call::Call(tcp::socket* sock) {
        _sock = sock;
    }


    bool Call::read() {
        if(_sock == NULL)
            return false;

        try{

            _class = Writable::readString(_sock);
            _method = Writable::readString(_sock);

//TODO:read param

        } catch(exception& e) {
            cout<<e.what()<<endl;
            return false;
        }

        return true;
    }

    bool Call::write() {
        //just write value back for now.
        value.write(_sock);

        return true;
    }


    void Call::print() {
        cout<<"class "<<_class<<" , method "<<_method<<endl;
    }


    Call::~Call()
    {
        //dtor
    }

}
