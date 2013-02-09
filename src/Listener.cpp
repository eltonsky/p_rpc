#include "Listener.h"

using boost::asio::ip::tcp;
using namespace std;

namespace Server {

    Listener::Listener(int port) :
        _bq_acpt_sock(max_accept_conns),
        _acceptor(_io_service_listener,
            tcp::endpoint(tcp::v4(), port))  {
        _port = port;

        for(int i=0; i<num_readers; i++) {
            _readers[i] = shared_ptr<Reader>(new Reader(i, max_reader_queue_size));
        }

        _last_reader_index= -1;
    }


    //start listener
    void Listener::run() {
        for(int i=0; i<num_readers; i++) {
            _readers[i].get()->run();
        }

        _t_listener = boost::thread(boost::bind(&Server::Listener::start,this));

        _t_listener.join();
    }


    void Listener::start(){

        printf("Listener started. <thread id : %ld>, <pid : %d> \n", (long int)syscall(SYS_gettid), getpid());

        if(!_should_stop) {
            _do_accept();
        }

        _io_service_listener.run();

        cout<<" _io_service_listener exit.. "<<endl;
    }


    void Listener::handle_accept(shared_ptr<tcp::socket>& sock,
      const boost::system::error_code& error)
    {
        cout<<"error: "<<error<<endl;

        if (!error && !_should_stop)
        {
            sock.get()->async_read_some(boost::asio::null_buffers(),
                boost::bind(&Listener::handle_read, this, sock,
                  boost::asio::placeholders::error,
                  boost::asio::placeholders::bytes_transferred));

            _do_accept();
        }
    }


    void Listener::handle_read(shared_ptr<tcp::socket> sock, const boost::system::error_code& error,
              size_t bytes_transferred)
    {
        cout<<"handle_read: error " << error<<endl;

        if (!error && !_should_stop)
        {
            //assign this socket to one of readers, roundrobin
            if(!addToReader(sock)) {
                throw "Readers are all full. Can not add sock";
            }
        }
    }


    bool Listener::addToReader(shared_ptr<tcp::socket> sock) {
        _last_reader_index = (_last_reader_index+1)%num_readers;
        short retry_times = 0;

        while(retry_times < num_readers) {
            if(_readers[_last_reader_index].get()->add(sock)) {
                cout<<"added to reader "<<_last_reader_index<<endl;

                return true;
            } else {
                cout<<"Failed to add to reader "<<_last_reader_index<<endl;
            }

            _last_reader_index = (_last_reader_index+1)%num_readers;
            retry_times++;
        }

        return false;
    }


    void Listener::stop() {
        try{

            // set stop flag for listener. stop accept.
            _should_stop = true;

            cout<<"wait for readers" <<endl;

            for(int i=0; i<num_readers; i++) {
                _readers[i].get()->waitToFinish();
            }

           _t_listener.interrupt();

           cout<<"listener finished" <<endl;

        } catch( exception& e ) {
            cout<<"Listener::stop() : "<<e.what()<<endl;
            exit(1);
        }

    }


    void Listener::join() {
        _t_listener.join();
    }


    void Listener::_do_accept() {
        if (!_should_stop)
        {
            _curr_sock = std::shared_ptr<tcp::socket>(new tcp::socket(_io_service_listener));

            _acceptor.async_accept(*(_curr_sock.get()),
                boost::bind(&Listener::handle_accept, this, _curr_sock,
                boost::asio::placeholders::error));
        }
    }


    Listener::~Listener()
    {
        cout<<"Listener destructor."<<endl;
    }

}

