#include "Handler.h"

using namespace std;

namespace Server {

    const int max_num_calls = 10;
    BlockQueue<std::shared_ptr<Call>> _bq_call(max_num_calls);
    atomic<int> last_call_id(-1);

    Handler::Handler(short id)
    {
        _handler_id = id;
        start();
    }

    void Handler::start() {
        _t_handler = boost::thread(boost::bind(&Server::Handler::handle, this));
    }

    void Handler::handle() {
        shared_ptr<Call> call;

        while(!_should_stop) {
            call = _bq_call.pop();

//print
cout<<"call in handler "<<_handler_id<<endl;
call.get()->print();

        }
    }


    void Handler::waitToFinish() {
        while(_bq_call.size() != 0) {
            this_thread::sleep_for(chrono::milliseconds(recheck_interval));
        }

        //stop producing any more responds
        _should_stop = true;

        _t_handler.interrupt();
    }


    Handler::~Handler()
    {
        //dtor
    }

}
