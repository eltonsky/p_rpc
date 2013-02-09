#include <cstdlib>
#include <iostream>
#include <sstream>
#include <thread>
#include <mutex>
#include "BlockQueue.h"
#include <sys/syscall.h>
#include <chrono>
#include <condition_variable>
#include "unistd.h"
#include "stdlib.h"
#include "Listener.h"
#include <signal.h>
#include "Utils.h"
#include "Server.h"

Server::Server* server_ptr;
atomic<bool> teminated(false);
const int server_port = 1234;

void terminate(int signum) {
    if(signum == SIGINT && teminated == false) {
        teminated.store(true);

        cout << " Terminating .. " << endl;

        long start = Utils::getTime();

        server_ptr->stop();

        server_ptr = NULL;

        cout << "Terminate takes " << Utils::getTime() - start << " miliseconds" <<endl;

        exit(0);
    } else {
        cout<<" SIGNAL "<<signum<<" ignored .."<<endl;
    }
}


int main()
{
    signal(SIGINT, terminate);

    try {

        Server::Server serv(server_port);

        server_ptr = &serv;

        serv.start();

    }  catch(exception& e) {
        cout<<e.what()<<endl;
    }

    cout << "Finish.." << endl;
    return 0;
}





