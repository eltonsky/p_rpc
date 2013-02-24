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
#include "Log.h"

Server::Server* server_ptr;
atomic<bool> teminated(false);
const int server_port = 1234;
const string log_conf = "./conf/log4cpp.properties";

void terminate(int signum) {
    if(signum == SIGINT && teminated == false) {
        teminated.store(true);

        Log::write(INFO, " Terminating .. \n");

        long start = Utils::getTime();

        server_ptr->stop();

        server_ptr = NULL;

        Log::write(INFO, "Terminate takes %ld miliseconds\n", (Utils::getTime() - start));

        exit(0);
    } else {
        Log::write(INFO, "SIGNAL %d ignored ..\n", signum);
    }
}


int main()
{
    signal(SIGINT, terminate);
    Log::init(log_conf);

    try {

        Server::Server serv(server_port);

        server_ptr = &serv;

        serv.start();

    }  catch(exception& e) {
        Log::write(INFO, e.what());
    }

    Log::write(INFO, "Finish !\n");
    return 0;
}





