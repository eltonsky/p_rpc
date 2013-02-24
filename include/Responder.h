#ifndef RESPONDER_H
#define RESPONDER_H

#include "BlockQueue.h"
#include "Call.h"
#include <boost/thread.hpp>
#include <sys/syscall.h>

using namespace std;

namespace Server{

    extern BlockQueue<std::shared_ptr<Call>> _bq_respond;

    class Responder
    {
        public:
            bool _should_stop = false;

            Responder();
            virtual ~Responder();

            void start();

            void respond();

            void waitToFinish();

        protected:
        private:
            boost::thread _t_responder;
    };

}
#endif // RESPONDER_H
