#include "Client.h"

Client::Client()
{
    //ctor
}

Client::Client(string valueClass, tcp::endpoint ep)
    : _valueClass(valueClass), _server_ep(ep),
    _bq_client_calls(_max_client_calls), _should_stop(false) {

    _t_recv_respond = boost::thread(boost::bind(&Client::start,this));
}


void Client::start() {
    while(!_should_stop) {
        if(waitForWork()) {

            recvRespond();

        } else {
            break;
        }
    }
}


void Client::recvRespond() {

}


bool Client::waitForWork() {
    std::unique_lock<std::mutex> ulock(_mutex_client);

    while(true){

        if(_cond_client.wait_for(ulock, chrono::milliseconds(_call_wait_time),
            [this] { return _bq_client_calls.size() > 0; })) {

            if(_should_stop)
                return false;
            else
                return true;
        } else if(_should_stop)
            return false;
    }
}


void Client::stop() {
    _should_stop = true;


}


Writable Client::call(Client::Call call) {
    assert(!call.getDone());

    if(!call.connect(_server_ep)) {
        Log::write(ERROR, "Can not connect to endpoint  %s : %d \n", _server_ep.address().to_string().c_str(), _server_ep.port());
        throw "Can not connect";
    }

    if(!_bq_client_calls.try_push(call)) {
        throw "FATAL: can not insert call into _bq_client_calls. is it full !?";
    }

    _cond_client.notify_all();

    Log::write(INFO, "_bq_client_calls.size() %d\n", _bq_client_calls.size());

    sendCall(std::ref(call));

    while(!_should_stop && !call.getDone()) {
        call.wait(_call_wait_time);
    }

    return call.getValue();
}


void Client::sendCall(Client::Call& call) {

}


Client::~Client()
{
    //dtor
}
