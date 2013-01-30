#include "Listener.h"

using boost::asio::ip::tcp;
using namespace std;

namespace Server {


    Listener::Listener(int port) :
        _bq_acpt_sock(max_accept_conns),
        _acceptor(_io_service_listener,
            tcp::endpoint(tcp::v4(), port)),
            _disp(Dispatcher(*this))  {
        _port = port;
    }


    void Listener::run() {
        _t_listener = boost::thread(boost::bind(&Server::Listener::start,this));

        //start dispatcher
        _t_disp = boost::thread(boost::bind(&Server::Listener::Dispatcher::start, _disp));
    }


    void Listener::start(){

        printf("Listener started. <thread id : %ld>, <pid : %d> \n", (long int)syscall(SYS_gettid), getpid());

        while(!_should_stop) {

            _curr_sock = std::shared_ptr<tcp::socket>(new tcp::socket(_io_service_disp));

            _acceptor.accept(*(_curr_sock.get()));

            _bq_acpt_sock.push(_curr_sock);
        }

    }


    void Listener::stop() {
        _should_stop = true;

        _t_disp.join();

cout<<"_t_disp joined " <<endl;

        for(vector<shared_ptr<Reader>>::iterator iter = _readers.begin();
            iter != _readers.end();iter++) {
            (*iter).get()->join();
        }

cout<<"wait for _t_listener" <<endl;

        _t_listener.join();

cout<<"_t_listener joined " <<endl;
    }


    void Listener::join() {
        _t_listener.join();
    }


    Listener::~Listener()
    {
        cout<<"Listener destructor."<<endl;
    }

}

