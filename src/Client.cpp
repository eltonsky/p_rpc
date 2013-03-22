//TODO: when exception, stop a call or a connection, instead of std::abort. it's a bit hash.

#include "Client.h"

const long Client::_call_wait_time = 100;
bool Client::_should_stop = false;

/// Client::Connection

Client::Connection::Connection(shared_ptr<tcp::endpoint> ep) :
                                _ep(ep) ,
                                _recv_started(false),
                                bq_conn_calls(_max_conn_calls) {}


bool Client::Connection::connect(shared_ptr<tcp::endpoint> ep,
                                 shared_ptr<Call> call) {
    try{
        _sock = new tcp::socket(_io_service);

        _sock->connect(*(ep.get()));

        Log::write(INFO, "Client connected to %s : %d\n",
                   ep->address().to_string().c_str(),
                   ep->port());

    } catch(...) {
        Log::write(ERROR, "Failed to connect to %s : %d\n",
                   ep->address().to_string().c_str(),
                    ep->port());

        return false;
    }

    if(!_recv_started) {
        try{
            // start recv thread
            _t_recv_respond =
                boost::thread(boost::bind(&Client::Connection::recvStart,this));

            _recv_started = true;

        }catch(exception& e){
            Log::write(ERROR, "Failed to start thread _t_recv_respond.\n");

            return false;
        }
    }

    return true;
}


Client::Connection::~Connection() {
}


void Client::Connection::recvStart() {
    Log::write(INFO, "Connection receiver thread started. <thread id : %ld>, <pid : %d> \n",
               (long int)syscall(SYS_gettid), getpid());

    while(waitForWork()) {

        Log::write(DEBUG, "Got calls in Connection\n");

        recvRespond();
    }

    Log::write(INFO, "Connection receiver thread exits.\n");
}


// bear in mind there's only one thread here playing with _calls
// from this function, so no lock is need here. great!
void Client::Connection::recvRespond() {

    int recv_call_id = -1;

    try{

        size_t l = boost::asio::read(*(_sock),
            boost::asio::buffer(&recv_call_id, sizeof(recv_call_id)));

        if(l<=0) {
            Log::write(ERROR, "Failed to read call_id from connection <%s>\n",
                        this->toString().c_str());
            std::abort();
        }

        Log::write(DEBUG, "recv_call_id is %d\n",recv_call_id);

        map<int,shared_ptr<Call>>::iterator iter= _calls.find(recv_call_id);

        if(iter == _calls.end()) {
            Log::write(ERROR, "Can not find received call id %d\n", recv_call_id);
            std::abort();
        }

        shared_ptr<Call> curr_call = iter->second;

        shared_ptr<Writable> val = Method::getNewInstance(curr_call->getValueClass());

        val->readFields(_sock);

        curr_call->setValue(val);

    }catch(exception& e){
        Log::write(ERROR, "Failed to read value for call %d and set value - %s\n",
                    curr_call->getId(), e.what());
    }
}


// this only returns when either:
// 1. should stop OR
// 2. get a call
// return - true : if get a call
//        - false: if should stop.
bool Client::Connection::waitForWork() {
    while(1) {
        std::unique_lock<std::mutex> ulock(_mutex_conn);

        if(_cond_conn.wait_for(ulock, chrono::milliseconds(_call_wait_time),
            [this] { return _calls.size() > 0; })) {

            return true;

        } else if(_should_stop)
            return false;
    }

    return false;
}


bool Client::Connection::addCall(shared_ptr<Call> call) {
    std::unique_lock<std::mutex> ulock(_mutex_conn);

    try{
        _calls.insert(pair<int,shared_ptr<Call>(call->getId(),call));

        _cond_conn.notify();

        Log::write(DEBUG, "insert a call to connection, _call.size() %d\n", call->size());

    } catch(...) {
        Log::write(ERROR, "Failed to insert %s to _calls\n", call->toString());
        return false;
    }

    return true;
}



/// Client

Client::Client(){}


void Client::stop() {
    Log::write(INFO, "Client receiver finished\n");
}


shared_ptr<Writable> Client::call(shared_ptr<Writable> param,
                                  string v_class,
                                  shared_ptr<tcp::endpoint> ep) {

    shared_ptr<Call> call(new Call(param, v_class));
    call->setId(conn->last_call_index++);

    shared_ptr<Connection> curr_conn;

    if((curr_conn = getConnection(ep, call)) == NULL) {
        Log::write(ERROR, "Can not connect to endpoint  %s : %d \n",
                   ep->address().to_string().c_str(),
                   ep->port());
        return NULL;
    }

    sendCall(call.get());

    Log::write(DEBUG, "curr_conn->bq_conn_calls.size() %d\n",
               curr_conn->bq_conn_calls.size());

    while(!_should_stop && !call->getDone()) {
        call->wait(_call_wait_time);
    }

    // close the underline socket in this conn.
    // a new socket will be created for a new conn.
    curr_conn->close();

    return call->getValue();
}


shared_ptr<Client::Connection> Client::getConnection(shared_ptr<tcp::endpoint> ep,
                                                     shared_ptr<Call> call) {
    std::unique_lock<std::mutex> ulock(_mutex_client);

    shared_ptr<Client::Connection> conn;

    try{
        map<shared_ptr<tcp::endpoint>,shared_ptr<Client::Connection>>::iterator iter =
            _connections.find(ep);

        if(iter == _connections.end()) {
            conn = make_shared<Client::Connection>(ep);

            _connections.insert(pair<shared_ptr<tcp::endpoint>,shared_ptr<Client::Connection>>(ep,conn));
        } else {
            conn = iter->second;

            Log::write(DEBUG, "Reuse connection obj\n");
        }

        conn->connect(ep,call);

    }catch(exception& e){
        Log::write(ERROR, "Failed to retrieve/create connection : %s\n", e.what());

        return NULL;
    }

    if(!conn->addCall(call)) {
        Log::write(ERROR, "FATAL: can not insert call into bq_conn_calls. is it full !?");
        return NULL;
    }

    call->setConnection(conn);

    return conn;
}


//TODO::move this to connection
void Client::sendCall(Call* call) {

    if(!call->write()) {
        Log::write(ERROR, "Client::sendCall: Failed\n");
    }
}


Client::~Client()
{
}




