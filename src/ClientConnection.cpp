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

weak_ptr<ClientConnection> ClientConnection::start(void)
{
    shared_ptr<ClientConnection> _this(this);
    this->thisSendingPtr = _this;
    this->thisReceivingPtr = _this;

    return _this;
}

void ClientConnection::sendingDone(void)
    throw()
{
    if (this->options.isAsync())
    {
        mutex::scoped_lock am(*this->clientSocketMutex);
        socket_utils::close(*this->clientSocket);
    }
    else
    {
        socket_utils::close(*this->clientSocket);
    }
    this->thisSendingPtr.reset();
}

void ClientConnection::receivingDone(void)
    throw()
{
    if (this->options.isAsync())
    {
        mutex::scoped_lock am(*this->backendSocketMutex);
        socket_utils::close(this->backendSocket);
    }
    else
    {
        socket_utils::close(this->backendSocket);
    }
    this->thisReceivingPtr.reset();
}

void ClientConnection::join(void)
{
    if (!this->options.isAsync())
    {
        this->sendingThread->join();
        this->receivingThread->join();
    }
}

bool ClientConnection::isReceiveError(const error_code &receiveError)
{
    if (receiveError)
    {
        if (receiveError != eof  &&
            receiveError != bad_descriptor  &&
            receiveError != operation_aborted)
        {
            cerr << "Error receiving from socket: " << receiveError.message() << endl;
        }
        return true;
    }
    return false;
}

bool ClientConnection::isSendError(const error_code &sendError)
{
    if (sendError)
    {
        cerr << "Error sending to socket: " << sendError.message() << endl;
        return true;
    }
    return false;
}


void ClientConnection::syncTransfer(tcp::socket &sourceSocket, tcp::socket &targetSocket)
    throw()
{
    char data[10240];
    error_code receiveError, sendError;

    do
    {
        size_t bytesRead = sourceSocket.read_some(buffer(data, sizeof(data)), receiveError);
        char *dataToSend = data;
        while (bytesRead > 0)
        {
            size_t bytesSent = targetSocket.write_some(buffer(dataToSend, bytesRead), sendError);
            bytesRead -= bytesSent;
            dataToSend += bytesSent / sizeof(*dataToSend);
        }
    }
    while (!isReceiveError(receiveError) && !isSendError(sendError));
}

void ClientConnection::syncSend(void)
{
    ClientConnection::syncTransfer(this->backendSocket, *this->clientSocket);
    this->sendingDone();
}

void ClientConnection::syncReceive(void)
{
    ClientConnection::syncTransfer(*this->clientSocket, this->backendSocket);
    this->receivingDone();
}

weak_ptr<ClientConnection> ClientConnection::syncStart(void)
{
    this->sendingThread.reset(new thread(&ClientConnection::syncSend, this));
    this->receivingThread.reset(new thread(&ClientConnection::syncReceive, this));

    return this->start();
}


void ClientConnection::asyncReceiveFromBackend(const error_code &sendError, size_t size)
{
    if (ClientConnection::isSendError(sendError))
    {
        this->sendingDone();
        return;
    }
    {
        mutex::scoped_lock am(*this->backendSocketMutex);
        this->backendSocket.async_read_some(buffer(this->sendingBuf.get(), ClientConnection::BUF_SIZE), bind(&ClientConnection::asyncSendToClient, this, placeholders::error, placeholders::bytes_transferred));
    }
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
    if (ClientConnection::isSendError(sendError))
    {
        this->receivingDone();
        return;
    }
    {
        mutex::scoped_lock am(*this->clientSocketMutex);
        this->clientSocket->async_read_some(buffer(this->receivingBuf.get(), ClientConnection::BUF_SIZE), bind(&ClientConnection::asyncSendToBackend, this, placeholders::error, placeholders::bytes_transferred));
    }
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
    this->sendingBuf.reset(new char[ClientConnection::BUF_SIZE / sizeof(char)]);
    this->receivingBuf.reset(new char[ClientConnection::BUF_SIZE / sizeof(char)]);

    this->clientSocketMutex.reset(new mutex);
    this->backendSocketMutex.reset(new mutex);

    this->asyncReceiveFromBackend(error_code(), 0);
    this->asyncReceiveFromClient(error_code(), 0);

    return this->start();
}


}
}
}
