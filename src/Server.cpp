#include "Server.hpp"
#include "ClientConnection.hpp"
#include "Client.hpp"

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/thread.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
using namespace boost;
using namespace boost::asio;
using namespace boost::asio::error;
using namespace boost::asio::ip;
using namespace boost::system;

#include <iostream>
#include <list>
#include <string>
using namespace std;


namespace org
{
namespace voidland
{
namespace ssh_cache
{


class ServerInternal
{
private:
    const Options &options;
    io_service ioService;
    ClientService clientService;
    scoped_ptr<tcp::acceptor> v6Acceptor;
    scoped_ptr<tcp::acceptor> v4Acceptor;
    list<weak_ptr<ClientConnection> > clientConnections;
    mutex clientConnectionsMutex;


    void acceptHandler(const error_code &error, shared_ptr<tcp::socket> socket, tcp::acceptor &acceptor);
    void asyncAcceptor(tcp::acceptor &acceptor);

    void signalHandler(const error_code &error, int signalNumber);

public:
    ServerInternal(const Options &options);
    void run(void);
};


void ServerInternal::acceptHandler(const error_code &err, shared_ptr<tcp::socket> socket, tcp::acceptor &acceptor)
{
    weak_ptr<ClientConnection> clientConn;

    if (err)
    {
        if (err == operation_aborted)
        {
            // The acceptor has been closed by singnalHandler(), so this is not an unexpected error.
            return;
        }
        cerr << "Cannot accept connection: " << err.message() << endl;
    }
    else
    {
        try
        {
            clientConn = ClientConnection::createAndStart(this->options, this->clientService, socket);
        }
        catch (const system_error &e)
        {
            cerr << "Cannot create client connection: " << e.what() << endl;
        }
    }

    this->asyncAcceptor(acceptor);

    {
        mutex::scoped_lock am(this->clientConnectionsMutex);

        list<weak_ptr<ClientConnection> >::iterator i = this->clientConnections.begin();
        while (i != this->clientConnections.end())
        {
            if (!i->lock())
            {
                this->clientConnections.erase(i++);
            }
            else
            {
                i++;
            }
        }

        if (!clientConn.expired())
        {
            this->clientConnections.push_back(clientConn);
        }
    }
}

void ServerInternal::asyncAcceptor(tcp::acceptor &acceptor)
{
    shared_ptr<tcp::socket> socket(new tcp::socket(acceptor.get_io_service()));
    acceptor.async_accept(*socket,
        bind(&ServerInternal::acceptHandler, this, placeholders::error, socket, ref(acceptor)));
}

void ServerInternal::signalHandler(const error_code &error, int signalNumber)
{
    if (this->v6Acceptor)
    {
        try
        {
            this->v6Acceptor->close();
        }
        catch (const system_error &e)
        {
            cerr << "Error closing IPv6 server socket: " << e.what() << endl;
        }
    }
    if (this->v4Acceptor)
    {
        try
        {
            this->v4Acceptor->close();
        }
        catch (const system_error &e)
        {
            cerr << "Error closing IPv4 server socket: " << e.what() << endl;
        }
    }
    this->ioService.stop();
}


ServerInternal::ServerInternal(const Options &options) :
    options(options), clientService(options, ioService)
{
}

void ServerInternal::run(void)
{
    try
    {
        this->v6Acceptor.reset(new tcp::acceptor(this->ioService, tcp::endpoint(tcp::v6(), this->options.getPort())));
    }
    catch (const system_error &e)
    {
        cerr << "Cannot create TCP server socket on port " << this->options.getPort() << ", protocol IPv6: " << e.what() << endl;
    }
    try
    {
        bool v4AlreadyBound = false;
        if (this->v6Acceptor)
        {
            v6_only v6OnlyOption;
            this->v6Acceptor->get_option(v6OnlyOption);
            v4AlreadyBound = !v6OnlyOption.value();
        }
        if (!v4AlreadyBound)
        {
            this->v4Acceptor.reset(new tcp::acceptor(this->ioService, tcp::endpoint(tcp::v4(), this->options.getPort())));
        }
    }
    catch (const system_error &e)
    {
        cerr << "Cannot create TCP server socket on port " << this->options.getPort() << ", protocol IPv4: " << e.what() << endl;
    }

    if (!this->v4Acceptor && !this->v6Acceptor)
    {
        throw runtime_error("Cannot create TCP server socket on port " + lexical_cast<string>(this->options.getPort()) + ".");
    }

    if (this->v6Acceptor)
    {
        this->asyncAcceptor(*this->v6Acceptor);
    }
    if (this->v4Acceptor)
    {
        this->asyncAcceptor(*this->v4Acceptor);
    }

    signal_set signals(this->ioService, SIGINT, SIGTERM);
    signals.async_wait(bind(&ServerInternal::signalHandler, this, placeholders::error, placeholders::signal_number));

    this->ioService.run();

    for (list<weak_ptr<ClientConnection> >::iterator i = this->clientConnections.begin(); i != this->clientConnections.end(); i++)
    {
        if (shared_ptr<ClientConnection> clientConn = i->lock())
        {
            clientConn->join();
        }
    }
    this->clientConnections.clear();
}


Server::Server(const Options &options) :
    options(options)
{
}

void Server::run(void)
{
    ServerInternal impl(this->options);
    impl.run();
}


}
}
}
