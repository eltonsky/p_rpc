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

Server::Listener* listener_ptr;
atomic<bool> teminated(false);

void terminate(int signum) {
    if(signum == SIGINT && teminated == false) {
        teminated.store(true);

        cout << " Terminating .. " << endl;

        long start = Utils::getTime();

        listener_ptr->stop();

        listener_ptr = NULL;

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

        Server::Listener listener(1234);

        listener_ptr = &listener;

        listener.run();

        listener.join();

    } catch(exception& e) {
        cout<<e.what()<<endl;
    }

    cout << "Finish.." << endl;
    return 0;
}





