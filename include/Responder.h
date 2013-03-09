#ifndef RESPONDER_H
#define RESPONDER_H

#include "BlockQueue.h"
#include "Call.h"
#include <boost/thread.hpp>
#include <sys/syscall.h>

using namespace std;

namespace Server{

    extern BlockQueue<int> _bq_respond;

    class Responder
    {
        public:
            bool _should_stop = false;

            Responder();
            ~Responder();

            void start();

            void respond();

            void waitToFinish();

            bool doRespond(shared_ptr<Call>);

            bool doAsyncWrite();

            bool processResponse(BlockQueue<Call> queue);

        protected:
        private:
            boost::thread _t_responder;

            std::mutex _mutex;
            std::condition_variable _cond;
    };

}
#endif // RESPONDER_H
