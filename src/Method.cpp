#include "Method.h"

Method::Method()
{
    //ctor
}


void Method::call(Server::Call* call) {
    string c = call->getClass();
    string m = call->getMethod();

    Log::write(INFO, "get a call: class %s, method %s\n", c.c_str(), m.c_str());

    if(c == "FSNameSpace") {
        if(m == "create") {
            IntWritable* iw = new IntWritable(21);
            call->setValue(shared_ptr<IntWritable>(iw));
        }
    }

}


Method::~Method()
{
    //dtor
}

