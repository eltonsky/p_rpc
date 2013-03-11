#include "Connection.h"
#include "Call.h"

namespace Server{

    int _last_connection_index;
    std::mutex _mutex_conns;
    std::condition_variable _cond_conns;
    map<shared_ptr<tcp::endpoint>, shared_ptr<Connection>> _connections;

    Connection::Connection(shared_ptr<tcp::socket> sock,
                   shared_ptr<tcp::endpoint> ep, int i) :
                   _sock(sock), _ep(ep), index(i) {}


    // write in a async fasion:
    // add the call back to queue if it's not fully sent
    int Connection::processResponse() {
        shared_ptr<Call> call;

        try {
            std::unique_lock<std::mutex> ulock(_mutex);

            call = respond_queue.pop();

            tcp::socket* sock = _sock.get();

            size_t wrote = 0;
            size_t length = call->getValue().length();
            size_t curr_pos = call->getPos();

            if(curr_pos == 0) {
                int call_id = call->getId();
                //assume call_id is always sent without prob.
                boost::asio::write(*(sock), boost::asio::buffer((const char*)&call_id, sizeof(call_id)));

            } else {

                if( curr_pos < 0 || curr_pos > length) {
                    Log::write(ERROR ,"Illegal pos value (%d) for call %d\n", curr_pos, call->getId());
                    return -1;
                }
            }

            const char* start = call->getValue().c_str() + curr_pos;

            wrote = sock->write_some(boost::asio::buffer(start, (length - curr_pos)));

            // if didn't write all data, add the call back to respond_queue
            // and move on to next connection.
            if(wrote < length - curr_pos) {
                call->setPos(wrote+curr_pos);

                if(!respond_queue.try_push(call)) {
                    Log::write(ERROR ,"FATAL: can not insert call into respond_queue. is it full !?\n");
                    return -1;
                }

                return 1;
            }

            return 0;

        } catch(exception& e){
            Log::write(ERROR, "Failed to process call in connection %d\n",index);
            return -1;
        }
    }


    void Connection::close() {}

}
