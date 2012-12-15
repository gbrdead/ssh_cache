#ifndef _SSH_CACHE_CLIENT_CONNECTION_HPP_
#define _SSH_CACHE_CLIENT_CONNECTION_HPP_


#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
using namespace boost::asio;
using namespace boost::asio::ip;
using namespace boost::system;
using namespace boost;

#include <utility>
using namespace std;


namespace org
{
namespace voidland
{
namespace ssh_cache
{


class ClientConnection
{
private:
    scoped_ptr<tcp::socket> socket;

    ClientConnection(tcp::socket *socket);
    void run(void);
    static void runThread(shared_ptr<ClientConnection> clientConn);

public:
    static pair<shared_ptr<thread>, weak_ptr<ClientConnection> > start(tcp::socket *socket);
    ~ClientConnection(void);
};


}
}
}


#endif
