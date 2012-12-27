#ifndef _SSH_CACHE_CLIENT_CONNECTION_HPP_
#define _SSH_CACHE_CLIENT_CONNECTION_HPP_

#include "Client.hpp"

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>


namespace org
{
namespace voidland
{
namespace ssh_cache
{


using namespace boost;
using namespace boost::asio::ip;
using namespace boost::system;
using namespace std;


class ClientConnection
{
private:
    static scoped_ptr<ClientService> clientService;
    static void initClientService(io_service &ioService);

    shared_ptr<tcp::socket> clientSocket;
    tcp::socket backendSocket;

    shared_ptr<thread> sendingThread;
    shared_ptr<thread> receivingThread;


    ClientConnection(shared_ptr<tcp::socket> socket)
        throw (system_error);

    void send(void);
    void receive(void);

    static void runSendingThread(shared_ptr<ClientConnection> &clientConn);
    static void runReceivingThread(shared_ptr<ClientConnection> &clientConn);

public:
    static weak_ptr<ClientConnection> createAndStart(shared_ptr<tcp::socket> socket)
        throw (system_error, thread_resource_error);
    void join(void);
};


}
}
}

#endif
