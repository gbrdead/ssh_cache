#ifndef _SSH_CACHE_SOCKET_UTILS_HPP_
#define _SSH_CACHE_SOCKET_UTILS_HPP_

#include <boost/asio.hpp>

#include <string>


namespace org
{
namespace voidland
{
namespace ssh_cache
{
namespace socket_utils
{


using namespace boost::asio::ip;
using namespace boost::system;
using namespace std;


void connect(tcp::socket &socket, const string &host, const string &service)
    throw (system_error);
void transfer(tcp::socket &sourceSocket, tcp::socket &targetSocket)
    throw();
void close(tcp::socket &socket)
    throw();
void close(tcp::acceptor &acceptor)
    throw();


}
}
}
}

#endif
