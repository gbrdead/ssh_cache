#include "EchoServer.hpp"
#include "TestUtils.hpp"

#include <iostream>


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


void EchoServer::processIncomingBuffer(tcp::socket &socket, asio::streambuf &buf)
{
    istream is(&buf);
    string line;
    getline(is, line);
    writeLine(socket, line);
}

EchoServer::EchoServer(unsigned short port) :
    TestServer(port)
{
}

EchoServer::~EchoServer(void)
{
}


}
}
}
}
}
