#ifndef _SSH_CACHE_CLIENT_CONNECTION_HPP_
#define _SSH_CACHE_CLIENT_CONNECTION_HPP_


#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <utility>


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
    shared_ptr<tcp::socket> socket;

    ClientConnection(shared_ptr<tcp::socket> socket);
    void run(void);
    static void runThread(shared_ptr<ClientConnection> clientConn);

public:
    static pair<shared_ptr<thread>, weak_ptr<ClientConnection> > start(shared_ptr<tcp::socket> socket);
};


}
}
}


#endif
