#ifndef HANDLER_H
#define HANDLER_H

#include "BlockQueue.h"
#include "Call.h"
#include <boost/thread.hpp>

namespace Server{

    extern atomic<int> last_call_id;
    extern BlockQueue<std::shared_ptr<Call>> _bq_call;

    class Handler
    {
        public:
            bool _should_stop = false;

            Handler(short id);
            virtual ~Handler();

            void start();

            void handle();

            void waitToFinish();

        protected:
        private:
            boost::thread _t_handler;
            short _handler_id;
    };
}

#endif // HANDLER_H
