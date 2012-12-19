#ifndef _SSH_CACHE_CLIENT_CONNECTION_HPP_
#define _SSH_CACHE_CLIENT_CONNECTION_HPP_


#include <boost/asio.hpp>
#include <boost/thread.hpp>
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


class ClientConnection
{
private:
    shared_ptr<tcp::socket> socket;
    shared_ptr<thread> sendingThread;
    shared_ptr<thread> receivingThread;


    ClientConnection(shared_ptr<tcp::socket> socket);

    void send(void);
    void receive(void);

    static void runSendingThread(shared_ptr<ClientConnection> &clientConn);
    static void runReceivingThread(shared_ptr<ClientConnection> &clientConn);

public:
    static weak_ptr<ClientConnection> createAndStart(shared_ptr<tcp::socket> socket);
    void join(void);
};


}
}
}


#endif
