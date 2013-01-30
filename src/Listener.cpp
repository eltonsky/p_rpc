#include "Listener.h"

using boost::asio::ip::tcp;
using namespace std;

namespace Server {

    Listener::Listener(int port) : _bq_acpt_sock(5),
                    _acceptor(_io_service_listener,
                              tcp::endpoint(tcp::v4(), port)) {
        //start dispatcher
        //Dispatcher disp(*this);
    }

    void Listener::start(){

        printf("Listener started. <thread id : %ld>, <pid : %d> \n", (long int)syscall(SYS_gettid), getpid());

        while(1) {

            _curr_sock = std::shared_ptr<tcp::socket>(new tcp::socket(_io_service_disp));

            _acceptor.accept(*(_curr_sock.get()));

            _bq_acpt_sock.push(_curr_sock);
        }

    }


    Listener::~Listener()
    {
        cout<<"Listener destructor."<<endl;
    }

}

