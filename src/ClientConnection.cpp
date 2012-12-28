#include "ClientConnection.hpp"
#include "SocketUtils.hpp"
#include "Constants.hpp"
using namespace org::voidland::ssh_cache::socket_utils;

#include <boost/lexical_cast.hpp>

#include <iostream>
#include <string>
using namespace std;


namespace org
{
namespace voidland
{
namespace ssh_cache
{


ClientConnection::ClientConnection(ClientService &clientService, shared_ptr<tcp::socket> socket)
    throw (system_error) :
        clientService(clientService),
        clientSocket(socket),
        backendSocket(socket->get_io_service())
{
    address clientAddr = this->clientSocket->remote_endpoint().address();
    shared_ptr<Client> client = this->clientService.getClient(clientAddr);

    if (client->getMitmAttacksCount() < INITIAL_MITM_ATTACKS_COUNT)
    {
        connect(this->backendSocket, FAKE_BACKEND_HOST, lexical_cast<string>(FAKE_BACKEND_PORT));
        client->addMitmAttack();
    }
    else
    {
        connect(this->backendSocket, REAL_BACKEND_HOST, lexical_cast<string>(REAL_BACKEND_PORT));
    }
    client->connected();
}

void ClientConnection::send(void)
{
    transfer(this->backendSocket, *this->clientSocket);
    close(*this->clientSocket);
}

void ClientConnection::receive(void)
{
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

weak_ptr<ClientConnection> ClientConnection::createAndStart(ClientService &clientService, shared_ptr<tcp::socket> socket)
    throw (system_error, thread_resource_error)
{
    shared_ptr<ClientConnection> clientConn(new ClientConnection(clientService, socket));
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
