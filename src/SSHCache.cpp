#include "ClientConnection.hpp"
#include "Constants.hpp"

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
using namespace boost;
using namespace boost::asio;
using namespace boost::asio::ip;
using namespace boost::system;

#include <iostream>
#include <list>
using namespace std;


namespace org
{
namespace voidland
{
namespace ssh_cache
{


static list<weak_ptr<ClientConnection> > clientConnections;
static mutex clientConnectionsMutex;

static void asyncAcceptor(tcp::acceptor &acceptor);

static void acceptHandler(const error_code &error, shared_ptr<tcp::socket> socket, tcp::acceptor &acceptor)
{
    if (error)
    {   // Most probably the acceptor is closed by singnalHandler(), so this is not an unexpected error.
        return;
    }

    weak_ptr<ClientConnection> clientConn;
    try
    {
        clientConn = ClientConnection::createAndStart(socket);
    }
    catch (const system_error &e)
    {
        cerr << "Cannot create client connection with backend " << BACKEND_HOST << ":" << BACKEND_PORT << ": " << e.what() << endl;
    }
    asyncAcceptor(acceptor);

    {
        mutex::scoped_lock am(clientConnectionsMutex);

        list<weak_ptr<ClientConnection> >::iterator i = clientConnections.begin();
        while (i != clientConnections.end())
        {
            if (!i->lock())
            {
                clientConnections.erase(i++);
            }
            else
            {
                i++;
            }
        }

        clientConnections.push_back(clientConn);
    }
}

static void asyncAcceptor(tcp::acceptor &acceptor)
{
    shared_ptr<tcp::socket> socket(new tcp::socket(acceptor.get_io_service()));
    acceptor.async_accept(*socket,
        bind(acceptHandler, placeholders::error, socket, ref(acceptor)));
}

static void signalHandler(const error_code &error, int signalNumber,
        shared_ptr<tcp::acceptor> v6Acceptor, shared_ptr<tcp::acceptor> v4Acceptor)
{
    if (v6Acceptor)
    {
        v6Acceptor->close();
    }
    if (v4Acceptor)
    {
        v4Acceptor->close();
    }
}

int main(void)
{
    io_service ioService;
    shared_ptr<tcp::acceptor> v6Acceptor;
    shared_ptr<tcp::acceptor> v4Acceptor;

    try
    {
        v6Acceptor.reset(new tcp::acceptor(ioService, tcp::endpoint(tcp::v6(), PROXY_PORT)));
    }
    catch (const system_error &e)
    {
        cerr << "Cannot create TCP server socket on port " << PROXY_PORT << ", protocol IPv6: " << e.what() << endl;
    }
    try
    {
        bool v4AlreadyBound = false;
        if (v6Acceptor)
        {
            v6_only v6OnlyOption;
            v6Acceptor->get_option(v6OnlyOption);
            v4AlreadyBound = !v6OnlyOption.value();
        }
        if (!v4AlreadyBound)
        {
            v4Acceptor.reset(new tcp::acceptor(ioService, tcp::endpoint(tcp::v4(), PROXY_PORT)));
        }
    }
    catch (const system_error &e)
    {
        cerr << "Cannot create TCP server socket on port " << PROXY_PORT << ", protocol IPv4: " << e.what() << endl;
    }

    if (v6Acceptor)
    {
        asyncAcceptor(*v6Acceptor);
    }
    if (v4Acceptor)
    {
        asyncAcceptor(*v4Acceptor);
    }
    if (v4Acceptor || v6Acceptor)
    {
        signal_set signals(ioService, SIGINT, SIGTERM);
        signals.async_wait(
            bind(signalHandler, placeholders::error, placeholders::signal_number, v6Acceptor, v4Acceptor));

        ioService.run();

        for (list<weak_ptr<ClientConnection> >::iterator i = clientConnections.begin(); i != clientConnections.end(); i++)
        {
            if (shared_ptr<ClientConnection> clientConn = i->lock())
            {
                clientConn->join();
            }
        }
    }
    return 0;
}


}
}
}

int main(void)
{
    return org::voidland::ssh_cache::main();
}
