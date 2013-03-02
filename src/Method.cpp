#include "Method.h"

Method::Method()
{
    //ctor
}


shared_ptr<Writable> Method::call(string c, string m, vector<shared_ptr<Writable>> params) {

    Log::write(INFO, "Method::call : class %s, method %s\n", c.c_str(), m.c_str());

    if(c == "FSNameSystem") {
        if(m == "create") {
            IntWritable* iw = new IntWritable(21);
            //call->setValue(shared_ptr<IntWritable>(iw));

/***simulate real call**/
//std::this_thread::sleep_for(chrono::milliseconds(200));

            return shared_ptr<IntWritable>(iw);
        }
    }

    return NULL;
}


shared_ptr<Writable> Method::getNewInstance(string class_) {
    if(class_ == "IntWritable") {
        return shared_ptr<Writable>(new IntWritable());
    } else if(class_ == "FileSystem") {

    } else if(class_ == "MethodWritable"){
        return shared_ptr<Writable>(new MethodWritable());

    }

    return shared_ptr<Writable>(new Writable());
}


Method::~Method()
{
    //dtor
}
