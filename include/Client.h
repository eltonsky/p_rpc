#ifndef CLIENT_H
#define CLIENT_H

#include "Writable.h"

class Client
{
    public:
        Client();
        Client(string valueClass, string server, int port);
        virtual ~Client();
    protected:
    private:
};

#endif // CLIENT_H
