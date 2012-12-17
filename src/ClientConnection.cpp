#include "ClientConnection.hpp"

#include <boost/bind.hpp>
using namespace boost;
using namespace boost::asio;

#include <iostream>
using namespace std;


namespace org
{
namespace voidland
{
namespace ssh_cache
{


ClientConnection::ClientConnection(shared_ptr<tcp::socket> socket) :
    socket(socket)
{
}

void ClientConnection::run(void)
{
    io_service::work work(this->socket->get_io_service());

    this_thread::sleep(posix_time::seconds(30));    // simulate work

    this->socket->close();
}


void ClientConnection::runThread(shared_ptr<ClientConnection> clientConn)
{
    clientConn->run();
}

pair<shared_ptr<thread>, weak_ptr<ClientConnection> > ClientConnection::start(shared_ptr<tcp::socket> socket)
{
    shared_ptr<ClientConnection> clientConn(new ClientConnection(socket));
    shared_ptr<thread> thr(new thread(runThread, clientConn));
    return pair<shared_ptr<thread>, weak_ptr<ClientConnection> >(thr, clientConn);
}


}
}
}
