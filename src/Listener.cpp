#include "Listener.h"

using boost::asio::ip::tcp;
using namespace std;


Listener::Listener(boost::asio::io_service& io_service_l,
                   boost::asio::io_service& io_service_d, int port)
            : _io_service_listener(io_service_l), _io_service_disp(io_service_d),
                _acceptor(io_service_l, tcp::endpoint(tcp::v4(), port)) {

    while(1) {
        _curr_sock = std::shared_ptr<tcp::socket>(new tcp::socket(io_service_d));

        _acceptor.accept(*(_curr_sock.get()));

        _bq_acpt_sock.push(_curr_sock);
    }

}


Listener::~Listener()
{

}



