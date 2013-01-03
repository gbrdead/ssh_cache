#include "TestUtils.hpp"

#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/scoped_ptr.hpp>
using namespace boost;
using namespace boost::asio;
using namespace boost::asio::ip;
using namespace boost::system;

#include <string>


namespace org
{
namespace voidland
{
namespace ssh_cache
{
namespace test
{


static bool isPortFree(unsigned short port)
{
    io_service ioService;
    {
        scoped_ptr<tcp::acceptor> v6Acceptor;
        scoped_ptr<tcp::acceptor> v4Acceptor;

        try
        {
            v6Acceptor.reset(new tcp::acceptor(ioService, tcp::endpoint(tcp::v6(), port)));
        }
        catch (const system_error &e)
        {
        }
        try
        {
            v4Acceptor.reset(new tcp::acceptor(ioService, tcp::endpoint(tcp::v4(), port)));
        }
        catch (const system_error &e)
        {
        }
        return v6Acceptor || v4Acceptor;
    }
}

list<unsigned short> findFreePorts(size_t count)
{
    list<unsigned short> freePorts;

    for (unsigned short port = 49152;
        port <= 65535 && port >= 49152  &&  freePorts.size() < count;
        port++)
    {
        if (isPortFree(port))
        {
            freePorts.push_back(port);
        }
    }
    for (unsigned short port = 1024;
        port < 49152  &&  freePorts.size() < count;
        port++)
    {
        if (isPortFree(port))
        {
            freePorts.push_back(port);
        }
    }
    if (freePorts.size() < count)
    {
        throw runtime_error("Cannot find " + lexical_cast<string>(count) + " free ports.");
    }
    return freePorts;
}

unsigned short findFreePort(void)
{
    return findFreePorts(1).front();
}

void findFreePorts(unsigned short &listenPort, unsigned short &realBackendPort, unsigned short &fakeBakendPort)
{
    list<unsigned short> freePorts = findFreePorts(3);
    listenPort = freePorts.front();
    freePorts.pop_front();
    realBackendPort = freePorts.front();
    freePorts.pop_front();
    fakeBakendPort = freePorts.front();
    freePorts.pop_front();
}


}
}
}
}
