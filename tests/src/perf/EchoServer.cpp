#include "EchoServer.hpp"
#include "TestUtils.hpp"

#include <string>
using namespace std;


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
    string line = utils::readLine(socket, buf);
    utils::writeLine(socket, line);
}

EchoServer::EchoServer(unsigned short port, unsigned asyncThreadCount) :
    TestServer(port, asyncThreadCount)
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
