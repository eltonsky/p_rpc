#include "Client.h"


Client::Client(tcp::endpoint ep)
    : _server_ep(ep),_should_stop(false), _bq_client_calls(_max_client_calls) {

    _t_recv_respond = boost::thread(boost::bind(&Client::start,this));
}


void Client::start() {
    Log::write(INFO, "Client receiver thread started. <thread id : %ld>, <pid : %d> \n", (long int)syscall(SYS_gettid), getpid());

    while(!_should_stop) {
        if(waitForWork()) {

            Log::write(DEBUG, "client got work.");

            recvRespond();

        } else {
            //should stop
            break;
        }
    }
}


void Client::recvRespond() {

    Call*_curr_call = _bq_client_calls.pop().get();

cout<<"_curr_call poped : "<< _curr_call->getParam()->toString()<<endl;

    _curr_call->setValue();
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
    while(_bq_client_calls.size() != 0) {
        this_thread::sleep_for(chrono::milliseconds(_call_wait_time));
    }

    //stop producing any more responds
    _should_stop = true;

    _t_recv_respond.interrupt();

    Log::write(INFO, "Client receiver finished\n");
}


shared_ptr<Writable> Client::call(shared_ptr<Writable> param, string v_class) {
    shared_ptr<Call> call(new Call(param, v_class));

    if(!call->connect(_server_ep)) {
        Log::write(ERROR, "Can not connect to endpoint  %s : %d \n",
                   _server_ep.address().to_string().c_str(), _server_ep.port());
        throw "Can not connect";
    }

    sendCall(call.get());

    if(!_bq_client_calls.try_push(call)) {
        throw "FATAL: can not insert call into _bq_client_calls. is it full !?";
    }

    _cond_client.notify_all();

    Log::write(INFO, "_bq_client_calls.size() %d\n",
               _bq_client_calls.size());

    while(!_should_stop && !call->getDone()) {
        call->wait(_call_wait_time);
    }

    return call->getValue();
}


void Client::sendCall(Call* call) {
    cout<<" enter sendCall "<<endl;
    cout<<"sendCall "<<call->getParam()->toString()<<endl;

    if (call->getParam()->write(call->getConnection()) < 0) {
        string msg("Failed to send call: ");
        msg.append(call->getParam()->toString());
        msg.append("\n");
        Log::write(ERROR, msg.c_str());
        throw msg;
    }
}


Client::~Client()
{
}




