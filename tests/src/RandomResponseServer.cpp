#include "RandomResponseServer.hpp"
#include "SocketUtils.hpp"
#include "TestUtils.hpp"

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
using namespace boost::asio::error;
using namespace boost::system;

#include <iostream>


namespace org
{
namespace voidland
{
namespace ssh_cache
{
namespace test
{


void RandomResponseServer::lineReadHandler(const error_code &err, size_t size, shared_ptr<tcp::socket> socket, shared_ptr<asio::streambuf> buf)
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
        istream is(buf.get());
        shared_ptr<string> line(new string);
        getline(is, *line);
        {
            mutex::scoped_lock am(this->incomingLinesMutex);
            this->incomingLines.push_back(line);
        }

        shared_ptr<string> responseLine(new string(generateRandomString()));
        {
            mutex::scoped_lock am(this->outgoingLinesMutex);
            this->outgoingLines.push_back(responseLine);

            write(*socket, buffer(responseLine->c_str(), responseLine->length() * sizeof(string::value_type)));
            static string newLine("\n");
            write(*socket, buffer(newLine.c_str(), newLine.length() * sizeof(string::value_type)));
        }
    }

    this->asyncLineReader(socket, buf);
}

void RandomResponseServer::asyncLineReader(shared_ptr<tcp::socket> socket, shared_ptr<asio::streambuf> buf)
{
    async_read_until(*socket, *buf, '\n',
        bind(&RandomResponseServer::lineReadHandler, this, placeholders::error, placeholders::bytes_transferred, socket, buf));
}

void RandomResponseServer::acceptHandler(const error_code &err, shared_ptr<tcp::socket> socket, tcp::acceptor &acceptor)
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

void RandomResponseServer::asyncAcceptor(tcp::acceptor &acceptor)
{
    shared_ptr<tcp::socket> socket(new tcp::socket(acceptor.get_io_service()));
    acceptor.async_accept(*socket,
        bind(&RandomResponseServer::acceptHandler, this, placeholders::error, socket, ref(acceptor)));
}


void RandomResponseServer::runIOServiceThread(void)
{
    this->ioService.run();
}

RandomResponseServer::RandomResponseServer(unsigned short port)
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

    this->ioServiceRunThread.reset(new thread(&RandomResponseServer::runIOServiceThread, this));
}

RandomResponseServer::~RandomResponseServer(void)
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

list<shared_ptr<string> > RandomResponseServer::getIncomingLines(void)
{
    mutex::scoped_lock am(this->incomingLinesMutex);
    return this->incomingLines;
}

list<shared_ptr<string> > RandomResponseServer::getOutgoingLines(void)
{
    mutex::scoped_lock am(this->outgoingLinesMutex);
    return this->outgoingLines;
}

}
}
}
}
