#include "ClientConnection.hpp"
#include "SocketUtils.hpp"

#include <boost/bind.hpp>
using namespace boost::asio;
using namespace boost::asio::error;

#include <iostream>
#include <string>
using namespace std;


namespace org
{
namespace voidland
{
namespace ssh_cache
{


ClientConnection::ClientConnection(const Options &options, ClientService &clientService, shared_ptr<tcp::socket> socket)
    throw (system_error) :
        options(options),
        clientService(clientService),
        clientSocket(socket),
        backendSocket(socket->get_io_service())
{
    address clientAddr = this->clientSocket->remote_endpoint().address();
    shared_ptr<Client> client = this->clientService.getClient(clientAddr);

    if (client->getMitmAttacksCount() < this->options.getInitialMitmAttacks())
    {
        socket_utils::connect(this->backendSocket, this->options.getFakeBackendHost(), this->options.getFakeBackendPort());
        client->addMitmAttack();
    }
    else
    {
        socket_utils::connect(this->backendSocket, this->options.getRealBackendHost(), this->options.getRealBackendPort());
    }
    client->connected();
}

weak_ptr<ClientConnection> ClientConnection::createAndStart(const Options &options, ClientService &clientService, shared_ptr<tcp::socket> socket)
    throw (system_error, thread_resource_error)
{
    ClientConnection *clientConn = new ClientConnection(options, clientService, socket);
    return options.isAsync() ? clientConn->asyncStart() : clientConn->syncStart();
}

void ClientConnection::join(void)
{
    if (!this->options.isAsync())
    {
        this->sendingThread->join();
        this->receivingThread->join();
    }
}

void ClientConnection::sendingDone(void)
    throw()
{
    socket_utils::close(*this->clientSocket);
    this->thisSendingPtr.reset();
}

void ClientConnection::receivingDone(void)
    throw()
{
    socket_utils::close(this->backendSocket);
    this->thisReceivingPtr.reset();
}


void ClientConnection::syncSend(void)
{
    socket_utils::transfer(this->backendSocket, *this->clientSocket);
    this->sendingDone();
}

void ClientConnection::syncReceive(void)
{
    socket_utils::transfer(*this->clientSocket, this->backendSocket);
    this->receivingDone();
}

weak_ptr<ClientConnection> ClientConnection::syncStart(void)
{
    shared_ptr<ClientConnection> _this(this);
    this->thisSendingPtr = _this;
    this->thisReceivingPtr = _this;

    this->sendingThread.reset(new thread(&ClientConnection::syncSend, this));
    this->receivingThread.reset(new thread(&ClientConnection::syncReceive, this));

    return _this;
}


bool ClientConnection::isReceiveError(const error_code &receiveError)
{
    if (receiveError)
    {
        if (receiveError != eof  &&             // The remote peer has closed the connection.
            receiveError != operation_aborted)  // Another thread has closed the source socket.
        {
            cerr << "Error receiving from socket: " << receiveError.message() << endl;
        }
        return true;
    }
    return false;
}

bool ClientConnection::isSsendError(const error_code &sendError)
{
    if (sendError)
    {
        cerr << "Error sending to socket: " << sendError.message() << endl;
        return true;
    }
    return false;
}

void ClientConnection::asyncReceiveFromBackend(const error_code &sendError, size_t size)
{
    if (ClientConnection::isSsendError(sendError))
    {
        this->sendingDone();
        return;
    }
    this->backendSocket.async_read_some(buffer(this->sendingBuf.get(), ClientConnection::BUF_SIZE), bind(&ClientConnection::asyncSendToClient, this, placeholders::error, placeholders::bytes_transferred));
}

void ClientConnection::asyncSendToClient(const error_code &receiveError, size_t size)
{
    if (ClientConnection::isReceiveError(receiveError))
    {
        this->sendingDone();
        return;
    }

    async_write(*this->clientSocket,  buffer(this->sendingBuf.get(), size), bind(&ClientConnection::asyncReceiveFromBackend, this, placeholders::error, placeholders::bytes_transferred));
}

void ClientConnection::asyncReceiveFromClient(const error_code &sendError, size_t size)
{
    if (ClientConnection::isSsendError(sendError))
    {
        this->receivingDone();
        return;
    }
    this->clientSocket->async_read_some(buffer(this->receivingBuf.get(), ClientConnection::BUF_SIZE), bind(&ClientConnection::asyncSendToBackend, this, placeholders::error, placeholders::bytes_transferred));
}

void ClientConnection::asyncSendToBackend(const error_code &receiveError, size_t size)
{
    if (ClientConnection::isReceiveError(receiveError))
    {
        this->receivingDone();
        return;
    }
    async_write(this->backendSocket, buffer(this->receivingBuf.get(), size), bind(&ClientConnection::asyncReceiveFromClient, this, placeholders::error, placeholders::bytes_transferred));
}

weak_ptr<ClientConnection> ClientConnection::asyncStart(void)
{
    shared_ptr<ClientConnection> _this(this);
    this->thisSendingPtr = _this;
    this->thisReceivingPtr = _this;

    this->sendingBuf.reset(new char[ClientConnection::BUF_SIZE / sizeof(char)]);
    this->receivingBuf.reset(new char[ClientConnection::BUF_SIZE / sizeof(char)]);

    this->asyncReceiveFromBackend(error_code(), 0);
    this->asyncReceiveFromClient(error_code(), 0);

    return _this;
}


}
}
}
