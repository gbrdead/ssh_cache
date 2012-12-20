#ifndef _SSH_CACHE_SERVER_HPP_
#define _SSH_CACHE_SERVER_HPP_

#include "ClientConnection.hpp"

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

#include <list>


namespace org
{
namespace voidland
{
namespace ssh_cache
{


using namespace boost;
using namespace boost::asio;
using namespace boost::asio::ip;
using namespace std;


class Server
{
private:
    io_service ioService;
    shared_ptr<tcp::acceptor> v6Acceptor;
    shared_ptr<tcp::acceptor> v4Acceptor;
    list<weak_ptr<ClientConnection> > clientConnections;
    mutex clientConnectionsMutex;


    void acceptHandler(const error_code &error, shared_ptr<tcp::socket> socket, tcp::acceptor &acceptor);
    void asyncAcceptor(tcp::acceptor &acceptor);

    void signalHandler(const error_code &error, int signalNumber);

public:
    void run(void);
};


}
}
}


#endif
