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

        cout<<"Listner started ..."<<endl;

        while(1) {

            cout<<"in loop.. "<<endl;

            _curr_sock = std::shared_ptr<tcp::socket>(new tcp::socket(_io_service_disp));

            _acceptor.accept(*(_curr_sock.get()));

            _bq_acpt_sock.push(_curr_sock);
        }

    }


    Listener::~Listener()
    {
        cout<<"Listener destructor .."<<endl;
    }

}

