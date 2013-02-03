#include "Handler.h"


namespace Server {

    const int max_num_calls = 10;
    BlockQueue<std::shared_ptr<Call>> _bq_call(max_num_calls);
    int last_call_id = -1;

    Handler::Handler()
    {
        //ctor
    }

    Handler::~Handler()
    {
        //dtor
    }

}
