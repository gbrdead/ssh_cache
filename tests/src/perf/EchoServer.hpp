#ifndef _SSH_CACHE_ECHO_SERVER_HPP_
#define _SSH_CACHE_ECHO_SERVER_HPP_

#include "TestServer.hpp"


namespace org
{
namespace voidland
{
namespace ssh_cache
{
namespace test
{
namespace performance
{

using namespace org::voidland::ssh_cache::test;


class EchoServer :
    public TestServer
{
protected:
    virtual void processIncomingBuffer(tcp::socket &socket, asio::streambuf &buf);

public:
    EchoServer(unsigned short port);
    virtual ~EchoServer(void);
};


}
}
}
}
}

#endif
