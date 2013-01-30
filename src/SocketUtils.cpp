#include "SocketUtils.hpp"

#include <boost/scoped_ptr.hpp>
using namespace boost;
using namespace boost::asio;
using namespace boost::asio::error;
using namespace boost::system;

#include <iostream>
using namespace std;


namespace org
{
namespace voidland
{
namespace ssh_cache
{
namespace socket_utils
{


void connect(tcp::socket &socket, const string &host, const string &service)
    throw (system_error)
{
    tcp::resolver resolver(socket.get_io_service());
    tcp::resolver::query query(host, service);

    bool ok = false;
    scoped_ptr<system_error> firstException;
    for (tcp::resolver::iterator i = resolver.resolve(query); i != tcp::resolver::iterator(); i++)
    {
        try
        {
            socket.connect(*i);
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


void transfer(tcp::socket &sourceSocket, tcp::socket &targetSocket)
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

        if (receiveError)
        {
            if (receiveError != eof  &&         // The remote peer has closed the connection.
                receiveError != bad_descriptor) // Another thread has closed sourceSocket.
            {
                cerr << "Error receiving from socket: " << receiveError.message() << endl;
            }
        }
        if (sendError)
        {
            cerr << "Error sending to socket: " << sendError.message() << endl;
        }
    }
    while (!receiveError && !sendError);
}

void close(tcp::socket &socket)
    throw()
{
    error_code ignoreError;
    socket.shutdown(tcp::socket::shutdown_both, ignoreError);
    socket.close(ignoreError);
}

void close(tcp::acceptor &acceptor)
    throw()
{
    error_code ignoreError;
    acceptor.close(ignoreError);
}


}
}
}
}
