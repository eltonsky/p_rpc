//TODO: when exception, stop a call or a connection, instead of std::abort. it's a bit hash.

#include "Client.h"

const long Client::_call_wait_time = 100;
bool Client::_should_stop = false;

Client::Client(tcp::endpoint ep)
    : _server_ep(ep){}


Client::Connection::Connection(tcp::endpoint ep){
    bool connected = true;

    try{
        tcp::resolver resolver(_io_service);
        tcp::resolver::query query(tcp::v4(), ep.address().to_string(), std::to_string(ep.port()));
        tcp::resolver::iterator iterator = resolver.resolve(query);

        _sock = new tcp::socket(_io_service);
        _sock->connect(*iterator);

        Log::write(INFO, "Client connected to %s : %d\n", ep.address().to_string().c_str(), ep.port());

    } catch(...) {
        Log::write(ERROR, "Failed to connect to %s : %d\n", ep.address().to_string().c_str(), ep.port());

        connected = false;
    }

    if(connected) {
        try{
            // start recv thread
            _t_recv_respond =
                boost::thread(boost::bind(&Client::Connection::recvStart,this));
        }catch(exception& e){
            Log::write(ERROR, "Failed to start thread _t_recv_respond.\n");
        }
    }
}


Client::Connection::~Connection() {
    if(_sock != NULL)
        delete _sock;
}


void Client::Connection::recvStart() {
    Log::write(INFO, "Connection receiver thread started. <thread id : %ld>, <pid : %d> \n", (long int)syscall(SYS_gettid), getpid());

    while(!_should_stop) {
        if(waitForWork()) {

            Log::write(DEBUG, "Got call in Connection\n");

            recvRespond();

        } else {
            //should stop
            break;
        }
    }
}


void Client::Connection::recvRespond() {

    shared_ptr<Call> curr_call = bq_conn_calls.pop();

    shared_ptr<Writable> val = curr_call->getValue();

    val = Method::getNewInstance(curr_call->getValueClass());

    try{
        val->readFields(_sock);

        curr_call->setValue(val);
    }catch(exception& e){
        Log::write(ERROR, "Failed to read value for call %d and set value.\n", curr_call->getId());
    }
}


bool Client::Connection::waitForWork() {
    std::unique_lock<std::mutex> ulock(_mutex_conn);

    while(true){

        if(_cond_conn.wait_for(ulock, chrono::milliseconds(_call_wait_time),
            [this] { return bq_conn_calls.size() > 0; })) {

            if(_should_stop)
                return false;
            else
                return true;
        } else if(_should_stop)
            return false;
    }
}


void Client::stop() {
    Log::write(INFO, "Client receiver finished\n");
}


shared_ptr<Writable> Client::call(shared_ptr<Writable> param, string v_class, tcp::endpoint ep) {

    shared_ptr<Call> call(new Call(param, v_class));
    shared_ptr<Connection> curr_conn;

    if((curr_conn = getConnection(ep, call)) == NULL) {
        Log::write(ERROR, "Can not connect to endpoint  %s : %d \n",
                   _server_ep.address().to_string().c_str(), _server_ep.port());
        return NULL;
    }

    sendCall(call.get());

    Log::write(INFO, "curr_conn->bq_conn_calls.size() %d\n",
               curr_conn->bq_conn_calls.size());

    while(!_should_stop && !call->getDone()) {
        call->wait(_call_wait_time);
    }

    return call->getValue();
}


shared_ptr<Client::Connection> Client::getConnection(tcp::endpoint ep, shared_ptr<Call> call) {
    std::unique_lock<std::mutex> ulock(_mutex_client);

    shared_ptr<Client::Connection> conn;

    try{
        map<tcp::endpoint,shared_ptr<Client::Connection>>::iterator iter = _connections.find(ep);

        if(iter == _connections.end()) {
            conn = make_shared<Client::Connection>(ep);

            _connections.insert(pair<tcp::endpoint,shared_ptr<Client::Connection>>(ep,conn));
        } else {
            conn = iter->second;
        }
    }catch(exception& e){
        Log::write(ERROR, "Failed to retrieve/create connection : %s\n", e.what());

        return NULL;
    }

    //TBD: if move following to a connetion function
    if(!conn->bq_conn_calls.try_push(call)) {
        Log::write(ERROR, "FATAL: can not insert call into bq_conn_calls. is it full !?");
        return NULL;
    }

    call->setId(conn->last_call_index++);
    call->setConnection(conn);

    return conn;
}


void Client::sendCall(Call* call) {

    if(!call->write()) {
        Log::write(ERROR, "Client::sendCall: Failed\n");
    }
}


Client::~Client()
{
}




