#include "Responder.h"

namespace Server{

    const int max_repsond_size = 100;
    BlockQueue<std::shared_ptr<Call>> _bq_respond(max_repsond_size);


    Responder::Responder()
    {
        //ctor
    }


    void Responder::start() {
        _t_responder = boost::thread(boost::bind(&Server::Responder::respond, this));
    }


    void Responder::respond() {
        Log::write(INFO, "Responder started. <thread id : %ld>, <pid : %d> \n", (long int)syscall(SYS_gettid), getpid());

        shared_ptr<Call> call;

        while(!_should_stop) {
            call = _bq_respond.pop();

            //print
            Log::write(DEBUG, "call in responder is %s\n", call.get()->toString().c_str());

            call.get()->write();
        }
    }


    void Responder::waitToFinish() {
        while(_bq_respond.size() != 0) {
            this_thread::sleep_for(chrono::milliseconds(recheck_interval));
        }

        //stop producing any more responds
        _should_stop = true;

        _t_responder.interrupt();

        Log::write(INFO, "responder finished\n");
    }


    Responder::~Responder()
    {
        //dtor
    }

}



