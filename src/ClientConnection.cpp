#include "ClientConnection.hpp"
#include "SocketUtils.hpp"
#include "Constants.hpp"
using namespace org::voidland::ssh_cache::socket_utils;

#include <boost/lexical_cast.hpp>
using namespace boost::asio;
using namespace boost::system;

#include <iostream>
#include <string>
using namespace std;


namespace org
{
namespace voidland
{
namespace ssh_cache
{


ClientConnection::ClientConnection(shared_ptr<tcp::socket> socket)
    throw (system_error)
        : clientSocket(socket), backendSocket(socket->get_io_service())
{
    connect(this->backendSocket, BACKEND_HOST, lexical_cast<string>(BACKEND_PORT));
}

void ClientConnection::send(void)
{
    io_service::work work(this->clientSocket->get_io_service());
    transfer(this->backendSocket, *this->clientSocket);
    close(*this->clientSocket);
}

void ClientConnection::receive(void)
{
    io_service::work work(this->clientSocket->get_io_service());
    transfer(*this->clientSocket, this->backendSocket);
    close(this->backendSocket);
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
    throw (system_error, thread_resource_error)
{
    shared_ptr<ClientConnection> clientConn(new ClientConnection(socket));
    clientConn->sendingThread.reset(new thread(&ClientConnection::runSendingThread, clientConn));
    clientConn->receivingThread.reset(new thread(&ClientConnection::runReceivingThread, clientConn));
    return clientConn;
}

void ClientConnection::join(void)
{
    this->sendingThread->join();
    this->receivingThread->join();
}


}
}
}
