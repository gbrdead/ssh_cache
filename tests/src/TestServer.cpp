#include "TestServer.hpp"
#include "SocketUtils.hpp"

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
using namespace boost::asio::error;
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
namespace test
{


void TestServer::lineReadHandler(const error_code &err, size_t size, shared_ptr<tcp::socket> socket, shared_ptr<asio::streambuf> buf)
{
    if (err)
    {
        if (err == eof)
        {
            socket_utils::close(*socket);
            return;
        }
        cerr << "Cannot read from socket: " << err.message() << endl;
    }
    else
    {
        this->processIncomingBuffer(*socket, *buf);
    }

    this->asyncLineReader(socket, buf);
}

void TestServer::asyncLineReader(shared_ptr<tcp::socket> socket, shared_ptr<asio::streambuf> buf)
{
    async_read_until(*socket, *buf, '\n',
        bind(&TestServer::lineReadHandler, this, placeholders::error, placeholders::bytes_transferred, socket, buf));
}

void TestServer::acceptHandler(const error_code &err, shared_ptr<tcp::socket> socket, tcp::acceptor &acceptor)
{
    if (err)
    {
        if (err == operation_aborted)
        {
            // The acceptor has been closed by the desctructor, so this is not an unexpected error.
            return;
        }
        cerr << "Cannot accept connection: " << err.message() << endl;
    }
    else
    {
        shared_ptr<asio::streambuf> buffer(new asio::streambuf);
        this->asyncLineReader(socket, buffer);
    }

    this->asyncAcceptor(acceptor);
}

void TestServer::asyncAcceptor(tcp::acceptor &acceptor)
{
    shared_ptr<tcp::socket> socket(new tcp::socket(acceptor.get_io_service()));
    acceptor.async_accept(*socket,
        bind(&TestServer::acceptHandler, this, placeholders::error, socket, ref(acceptor)));
}


void TestServer::runIOServiceThread(void)
{
    this->ioService.run();
}

TestServer::TestServer(unsigned short port)
{
    try
    {
        this->v6Acceptor.reset(new tcp::acceptor(this->ioService, tcp::endpoint(tcp::v6(), port)));
    }
    catch (const system_error &e)
    {
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
            this->v4Acceptor.reset(new tcp::acceptor(this->ioService, tcp::endpoint(tcp::v4(), port)));
        }
    }
    catch (const system_error &e)
    {
    }

    if (!this->v4Acceptor && !this->v6Acceptor)
    {
        throw system_error(address_in_use, "Cannot create TCP server socket on port " + lexical_cast<string>(port) + ".");
    }

    if (this->v6Acceptor)
    {
        this->asyncAcceptor(*this->v6Acceptor);
    }
    if (this->v4Acceptor)
    {
        this->asyncAcceptor(*this->v4Acceptor);
    }

    this->ioServiceRunThread.reset(new thread(&TestServer::runIOServiceThread, this));
}

TestServer::~TestServer(void)
{
    if (this->v6Acceptor)
    {
        socket_utils::close(*this->v6Acceptor);
    }
    if (this->v4Acceptor)
    {
        socket_utils::close(*this->v4Acceptor);
    }
    this->ioService.stop();
    this->ioServiceRunThread->join();
}

}
}
}
}
