#ifndef HANDLER_H
#define HANDLER_H

#include "BlockQueue.h"
#include "Call.h"

namespace Server{

    extern BlockQueue<std::shared_ptr<Call>> _bq_call;
    extern int last_call_id;

    class Handler
    {
        public:
            Handler();
            virtual ~Handler();

            void start();

        protected:
        private:
    };

}

#endif // HANDLER_H
