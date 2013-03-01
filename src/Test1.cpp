#include "Test1.h"

using namespace std;

Test1::Test1()
{
    //ctor
}

bool Test1::test1(string server_host, int port) {
    // get endponit
    boost::system::error_code error;
    boost::asio::ip::address_v4 targetIP;
    targetIP = boost::asio::ip::address_v4::from_string(server_host.c_str(), error);

    tcp::endpoint ep;
    ep.address(targetIP);
    ep.port(port);

    //create client
    Client client(ep);

    //get param
    vector<shared_ptr<Writable>> params;
    params.push_back(shared_ptr<IntWritable>(new IntWritable(10)));
    params.push_back(shared_ptr<IntWritable>(new IntWritable(20)));
    MethodWritable* mw = new MethodWritable("FSNameSystem", "create", params);
    shared_ptr<Writable> w = shared_ptr<MethodWritable>(mw);

//    Client::Call* call = new Client::Call(w, "IntWritable");
//    client.call(call);
//    shared_ptr<Writable> val = client.call(call);
//    Log::write(INFO, "In thread %ld, client received value : %s\n" , (long int)syscall(SYS_gettid), val->toString().c_str());
//    delete call;

    client.call(shared_ptr<Client::Call>(new Client::Call(w, "IntWritable")));

    return true;
}


bool Test1::test2(string server_host, int port) {
    boost::thread t1 = boost::thread(boost::bind(&Test1::test1, this, server_host, port));
    //boost::thread t2 = boost::thread(boost::bind(&Test1::test1, this, server_host, port));
    //boost::thread t3 = boost::thread(boost::bind(&Test1::test1, this, server_host, port));

    t1.join();
    //t2.join();
    //t3.join();

    return true;
}


Test1::~Test1()
{
    //dtor
}




