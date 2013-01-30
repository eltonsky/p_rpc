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

int main()
{
    try {
        Server::Listener listener(1234);
        boost::thread l(boost::bind(&Server::Listener::start, &listener));

        Server::Dispatcher disp(listener);
        boost::thread d(boost::bind(&Server::Dispatcher::start, &disp));

        disp.join();
        l.join();

    } catch(exception& e) {
        cout<<e.what()<<endl;
    }

    cout << "Finish.." << endl;
    return 0;
}





