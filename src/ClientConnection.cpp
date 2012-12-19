#include "ClientConnection.hpp"

using namespace boost::asio;


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

void ClientConnection::send(void)
{
    io_service::work work(this->socket->get_io_service());

    this_thread::sleep(posix_time::seconds(5));    // simulate work
}

void ClientConnection::receive(void)
{
    io_service::work work(this->socket->get_io_service());

    this_thread::sleep(posix_time::seconds(10));    // simulate work

    sendingThread->join();
    this->socket->close();
}

void ClientConnection::runSendingThread(shared_ptr<ClientConnection> &clientConn)
{
    clientConn->send();
    clientConn.reset(); // Breaks the cycle between clientConn and clientConn->sendingThread.
}

void ClientConnection::runReceivingThread(shared_ptr<ClientConnection> &clientConn)
{
    clientConn->receive();
    clientConn.reset(); // Breaks the cycle between clientConn and clientConn->receivingThread.
}

weak_ptr<ClientConnection> ClientConnection::createAndStart(shared_ptr<tcp::socket> socket)
{
    shared_ptr<ClientConnection> clientConn(new ClientConnection(socket));
    clientConn->sendingThread.reset(new thread(runSendingThread, clientConn));
    clientConn->receivingThread.reset(new thread(runReceivingThread, clientConn));
    return clientConn;
}

void ClientConnection::join(void)
{
    this->receivingThread->join();
}


}
}
}
