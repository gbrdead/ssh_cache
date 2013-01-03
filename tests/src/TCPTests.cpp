#include "Server.hpp"

#include "TestUtils.hpp"

#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/system/system_error.hpp>
using namespace boost;
using namespace boost::asio;
using namespace boost::asio::ip;
using namespace boost::system;
using namespace boost::system::errc;

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


BOOST_AUTO_TEST_CASE(PortAlreadyInUseTest)
{
    unsigned short listenPort = findFreePort();
    io_service ioService;
    {
        scoped_ptr<tcp::acceptor> v6Acceptor;
        scoped_ptr<tcp::acceptor> v4Acceptor;

        try
        {
            v6Acceptor.reset(new tcp::acceptor(ioService, tcp::endpoint(tcp::v6(), listenPort)));
        }
        catch (const system_error &e)
        {
        }
        try
        {
            v4Acceptor.reset(new tcp::acceptor(ioService, tcp::endpoint(tcp::v4(), listenPort)));
        }
        catch (const system_error &e)
        {
        }

        string listenPortAsString = lexical_cast<string>(listenPort);
        const char *argv[] =
        {
            "ssh_cache_tests",
            "--port",
            listenPortAsString.c_str()
        };
        int argc = sizeof(argv) / sizeof(argv[0]);
        Options options(argc, argv);

        Server server(options);
        try
        {
            server.run();
            BOOST_FAIL("boost::system_error exception expected.");
        }
        catch (const system_error &e)
        {
            BOOST_REQUIRE_EQUAL(e.code(), address_in_use);
        }
    }
}


}
}
}
}
