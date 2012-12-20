#include "ClientConnection.hpp"
#include "Constants.hpp"

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
    tcp::resolver resolver(this->clientSocket->get_io_service());
    tcp::resolver::query query(BACKEND_HOST, lexical_cast<string>(BACKEND_PORT));

    bool ok = false;
    scoped_ptr<system_error> firstException;
    for (tcp::resolver::iterator i = resolver.resolve(query); i != tcp::resolver::iterator(); i++)
    {
        try
        {
            this->backendSocket.connect(*i);
            ok = true;
            break;
        }
        catch (const system_error &e)
        {
            if (!firstException)
            {
                firstException.reset(new system_error(e));
            }
        }
    }
    if (!ok)
    {
        throw system_error(*firstException);
    }
}

void ClientConnection::send(void)
{
    io_service::work work(this->clientSocket->get_io_service());

    this_thread::sleep(posix_time::seconds(5));    // simulate work
}

void ClientConnection::receive(void)
{
    io_service::work work(this->clientSocket->get_io_service());

    this_thread::sleep(posix_time::seconds(10));    // simulate work

    this->sendingThread->join();
    try
    {
        this->clientSocket->close();
    }
    catch (const system_error &e)
    {
        cerr << "Error closing client connection socket: " << e.what() << endl;
    }
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
    throw (system_error)
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
