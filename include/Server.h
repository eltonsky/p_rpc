#ifndef SERVER_H
#define SERVER_H

#include "Listener.h"
#include "Handler.h"

namespace Server{

    const short num_handlers = 2;

    class Server
    {
        public:
            Server(int);
            virtual ~Server();

            void start();

            void stop();

        protected:
        private:
            Listener _listener;
            shared_ptr<Handler> _handlers[num_handlers];

    };

}

#endif // SERVER_H
