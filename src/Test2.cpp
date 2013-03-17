#include "Test2.h"

Test2::Test2()
{
    //ctor
}

Test2::~Test2()
{
    //dtor
}


void Test2::test21(string server_host, int port) {
    DataNodeProtocol namenode(server_host, port);

    shared_ptr<IntWritable> a1 = make_shared<IntWritable>(11);
    shared_ptr<IntWritable> a2 = make_shared<IntWritable>(22);
    shared_ptr<IntWritable> a3 = make_shared<IntWritable>(33);
    shared_ptr<IntWritable> a4 = make_shared<IntWritable>(44);

    shared_ptr<Writable> res = namenode.create(a1,a2,a3,a4);

    cout<<"res is "<<res->printToString()<<endl;
}
