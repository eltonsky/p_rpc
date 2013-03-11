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

        Log::write(INFO, "Listener started. <thread id : %ld>, <pid : %d> \n", (long int)syscall(SYS_gettid), getpid());

        if(!_should_stop) {
            _do_accept();
        }

        _io_service_listener.run();

        Log::write(INFO,"_io_service_listener exit.. \n");
    }


    void Listener::handle_accept(shared_ptr<tcp::socket> sock, shared_ptr<tcp::endpoint> ep,
      const boost::system::error_code& error)
    {
        Log::write(DEBUG, "Listener::handle_accept, error %s\n", error.message().c_str());

        if (!error && !_should_stop)
        {
            sock.get()->async_read_some(boost::asio::null_buffers(),
                boost::bind(&Listener::handle_read, this, sock, ep,
                  boost::asio::placeholders::error,
                  boost::asio::placeholders::bytes_transferred));

            _do_accept();
        }
    }


    void Listener::handle_read(shared_ptr<tcp::socket> sock, shared_ptr<tcp::endpoint> ep,
            const boost::system::error_code& error, size_t bytes_transferred)
    {
        Log::write(DEBUG, "Listener::handle_read: error %s\n", error.message().c_str());

        if (!error && !_should_stop)
        {
            //create or retrieve this connection
            shared_ptr<Connection> conn;

            try{

                _mutex_conns.lock();

                map<shared_ptr<tcp::endpoint>, shared_ptr<Connection>>::iterator iter =
                    _connections.find(ep);

                if (iter == _connections.end()) {
                    conn = make_shared<Connection>(sock, ep, _last_connection_index++);

                    _connections.insert(pair<shared_ptr<tcp::endpoint>,shared_ptr<Connection>>(ep, conn));

                } else {
                    conn = iter->second;
                }

                _mutex_conns.unlock();

                //assign this socket to one of readers, roundrobin
                if(!addToReader(conn)) {
                    throw "Readers are all full. Can not add sock";
                }

            }catch(exception& e){
                _mutex_conns.unlock();
                Log::write(ERROR, "Failed to read a call\n.");
            }

        }
    }


    bool Listener::addToReader(shared_ptr<Connection> conn) {
        _last_reader_index = (_last_reader_index+1)%num_readers;
        short retry_times = 0;

        while(retry_times < num_readers) {
            if(_readers[_last_reader_index].get()->add(conn)) {
                Log::write(DEBUG, "added to reader,  _last_reader_index %d\n", _last_reader_index);

                return true;
            } else {
                Log::write(ERROR, "Failed to add to reader! _last_reader_index %d\n", _last_reader_index);
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

            Log::write(INFO, "wait for readers\n");

            for(int i=0; i<num_readers; i++) {
                _readers[i].get()->waitToFinish();
            }

           _t_listener.interrupt();

            Log::write(INFO, "listener finished\n");

        } catch( exception& e ) {
            Log::write(ERROR, "Listener::stop() : %s\n", e.what());
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

            shared_ptr<tcp::endpoint> ep = make_shared<tcp::endpoint>();

            _acceptor.async_accept(*(_curr_sock.get()),
                *(ep.get()),
                boost::bind(&Listener::handle_accept, this, _curr_sock,
                ep, boost::asio::placeholders::error));
        }
    }


    Listener::~Listener()
    {
        cout<<"Listener destructor."<<endl;
    }

}

