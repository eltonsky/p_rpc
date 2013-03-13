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
        _sock = new tcp::socket(call->getIoService());

//        boost::asio::socket_base::linger option(true,0);
//        _sock->set_option(option);

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
    if(_sock != NULL)
        delete _sock;
}


void Client::Connection::recvStart() {
    Log::write(INFO, "Connection receiver thread started. <thread id : %ld>, <pid : %d> \n",
               (long int)syscall(SYS_gettid), getpid());

    while(!_should_stop) {
        shared_ptr<Call> curr_call = bq_conn_calls.pop();

        Log::write(DEBUG, "Got call in Connection\n");

        recvRespond(curr_call);
    }

    Log::write(INFO, "Connection receiver thread exits.\n");
}


void Client::Connection::recvRespond(shared_ptr<Call> curr_call) {

    Log::write(DEBUG, "receiving respond for call id %d\n", curr_call->getId());

    shared_ptr<Writable> val = Method::getNewInstance(curr_call->getValueClass());

    int curr_call_id = -1;

    try{

        size_t l = boost::asio::read(*(_sock),
            boost::asio::buffer(&curr_call_id, sizeof(curr_call_id)));

        if(l<=0) {
            Log::write(ERROR, "Failed to read call_id from connection <%s>\n",
                        this->toString().c_str());
            std::abort();
        }else if(curr_call_id != curr_call->getId()) {
            Log::write(ERROR,
                       "Unmatched call id. curr call id is %d, received call id is %d.\n",
                        curr_call->getId(), curr_call_id);
            std::abort();
        }

        Log::write(DEBUG, "curr_call_id is %d, curr_call's id is %d\n",
                    curr_call_id, curr_call->getId());

        val->readFields(_sock);

        curr_call->setValue(val);

    }catch(exception& e){
        Log::write(ERROR, "Failed to read value for call %d and set value - %s\n",
                    curr_call->getId(), e.what());
    }
}


//not used.
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


/// Client

Client::Client(shared_ptr<tcp::endpoint> ep)
    : _server_ep(ep){}


void Client::stop() {
    Log::write(INFO, "Client receiver finished\n");
}


shared_ptr<Writable> Client::call(shared_ptr<Writable> param,
                                  string v_class,
                                  shared_ptr<tcp::endpoint> ep) {

    shared_ptr<Call> call(new Call(param, v_class));
    shared_ptr<Connection> curr_conn;

    if((curr_conn = getConnection(ep, call)) == NULL) {
        Log::write(ERROR, "Can not connect to endpoint  %s : %d \n",
                   _server_ep->address().to_string().c_str(),
                   _server_ep->port());
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




