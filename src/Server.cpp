#include "Server.h"

namespace Server {

    Server::Server(int port) : _listener(port)
    {
        for(short i=0;i<num_handlers;i++) {
            _handlers[i] = shared_ptr<Handler>(new Handler(i));
        }
    }

    void Server::start() {
        for(short i=0;i<num_handlers;i++) {
            _handlers[i].get()->start();
        }

        _listener.run();
    }


    void Server::stop() {
        _listener.stop();

        for(short i=0;i<num_handlers;i++) {
            _handlers[i].get()->waitToFinish();
        }
    }


    Server::~Server()
    {
        //dtor
    }

}
