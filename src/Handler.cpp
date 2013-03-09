#include "Handler.h"

using namespace std;

namespace Server {

    const int max_num_calls = 100;
    BlockQueue<std::shared_ptr<Call>> _bq_call(max_num_calls);
    //atomic<int> last_call_id(-1);

    Handler::Handler(short id)
    {
        _handler_id = id;
    }

    void Handler::start() {
        _t_handler = boost::thread(boost::bind(&Server::Handler::handle, this));
    }


    void Handler::handle() {
        Log::write(INFO, "Handler %d started. <thread id : %ld>, <pid : %d> \n", _handler_id, (long int)syscall(SYS_gettid), getpid());

        shared_ptr<Call> call;

        while(!_should_stop) {
            call = _bq_call.pop();

            Log::write(DEBUG, "call in handler %d, call is %s\n", _handler_id, call.get()->toString().c_str());

            //call
            shared_ptr<Writable> res =
                Method::call(call->getClass(), call->getMethod(), call->getParams());

            call->setValue(res);

            Log::write(INFO, "value is %s\n", res->toString().c_str());

            //add res call to connection.respondQ
            _responder.doRespond(call);

            if(!_bq_respond.try_push(call->getConnection()->index)) {
                throw "FATAL: can not insert call's connection index into _bq_respond. is it full !?";
            }

            Log::write(INFO, "_bq_respond.size() %d\n", _bq_respond.size());
        }
    }


    void Handler::waitToFinish() {
        while(_bq_call.size() != 0) {
            this_thread::sleep_for(chrono::milliseconds(recheck_interval));
        }

        //stop producing any more responds
        _should_stop = true;

        _t_handler.interrupt();

        Log::write(INFO, "handler %d finished\n",_handler_id);
    }


    Handler::~Handler()
    {
        //dtor
    }

}
