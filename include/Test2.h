#ifndef TEST2_H
#define TEST2_H

#include <string>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <thread>
#include <memory>
#include "DataNodeProtocol.h"

class Test2
{
    public:
        Test2();
        ~Test2();

        void test21(string server_host, int port, int rep =1) ;
        bool test20(string server_host, int port, int thread=1, int rep=1);
    protected:
    private:

};

#endif // TEST2_H
