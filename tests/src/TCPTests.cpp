#include "Server.hpp"

#include "RandomResponseServer.hpp"
#include "TestUtils.hpp"

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/random.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/system/system_error.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/thread.hpp>
using namespace boost;
using namespace boost::asio;
using namespace boost::asio::ip;
using namespace boost::posix_time;
using namespace boost::random;
using namespace boost::system;
using namespace boost::system::errc;
using namespace boost::this_thread;

#include <list>
#include <string>
using namespace std;

extern "C"
{
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
}


namespace org
{
namespace voidland
{
namespace ssh_cache
{
namespace test
{


class ServerRunner
{
private:
    Server server;
    thread runServerThread;

    static void runServerThreadProc(Server &server);

public:
    ServerRunner(const Options &options);
    ~ServerRunner(void);
};

ServerRunner::ServerRunner(const Options &options) :
    server(options), runServerThread(&runServerThreadProc, ref(server))
{
    this->server.ensureRunning();
}

void ServerRunner::runServerThreadProc(Server &server)
{
    server.run();
}

ServerRunner::~ServerRunner(void)
{
    kill(getpid(), SIGTERM);
    this->runServerThread.join();
}


BOOST_AUTO_TEST_CASE(PortAlreadyInUseTest)
{
    unsigned short listenPort = utils::findFreePort();
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

void generalTest(bool async)
{
    unsigned short listenPort, realBackendPort, fakeBakendPort;
    utils::findFreePorts(listenPort, realBackendPort, fakeBakendPort);

    mt19937 gen(time(0));
    uniform_int_distribution<unsigned> connCountDist(1, 10);
    unsigned fakeConnectionsCount = connCountDist(gen);
    unsigned realConnectionsCount = connCountDist(gen);

    string listenPortAsString = lexical_cast<string>(listenPort);
    string realBackendPortAsString = lexical_cast<string>(realBackendPort);
    string fakeBackendPortAsString = lexical_cast<string>(fakeBakendPort);
    string mitmAttacksAsString = lexical_cast<string>(fakeConnectionsCount);
    const char *argv[] =
    {
        "ssh_cache_tests",
        "--port",
        listenPortAsString.c_str(),
        "--real-backend-host",
        "localhost",
        "--real-backend-port",
        realBackendPortAsString.c_str(),
        "--fake-backend-host",
        "localhost",
        "--fake-backend-port",
        fakeBackendPortAsString.c_str(),
        "--initial-mitm-attacks",
        mitmAttacksAsString.c_str(),
        "--client-expiration",
        "2147483647",
        "--async"
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - (async ? 0 : 1);
    Options options(argc, argv);
    ServerRunner serverRunner(options);

    RandomResponseServer realBackendServer(realBackendPort);
    RandomResponseServer fakeBackendServer(fakeBakendPort);

    list<shared_ptr<string> > fakeIncomingLines, fakeOutgoingLines;
    for (unsigned i = 0; i < fakeConnectionsCount; i++)
    {
        utils::transferSomeLines("localhost", listenPort, fakeIncomingLines, fakeOutgoingLines);
    }
    list<shared_ptr<string> > realIncomingLines, realOutgoingLines;
    for (unsigned i = 0; i < realConnectionsCount; i++)
    {
        utils::transferSomeLines("localhost", listenPort, realIncomingLines, realOutgoingLines);
    }

    BOOST_REQUIRE(fakeIncomingLines == fakeBackendServer.getOutgoingLines());
    BOOST_REQUIRE(fakeOutgoingLines == fakeBackendServer.getIncomingLines());
    BOOST_REQUIRE(realIncomingLines == realBackendServer.getOutgoingLines());
    BOOST_REQUIRE(realOutgoingLines == realBackendServer.getIncomingLines());
}

BOOST_AUTO_TEST_CASE(GeneralSyncTest)
{
    generalTest(false);
}

BOOST_AUTO_TEST_CASE(GeneralAsyncTest)
{
    generalTest(true);
}

BOOST_AUTO_TEST_CASE(ClientExpirationTest)
{
    unsigned short listenPort, realBackendPort, fakeBakendPort;
    utils::findFreePorts(listenPort, realBackendPort, fakeBakendPort);

    string listenPortAsString = lexical_cast<string>(listenPort);
    string realBackendPortAsString = lexical_cast<string>(realBackendPort);
    string fakeBackendPortAsString = lexical_cast<string>(fakeBakendPort);
    string mitmAttacksAsString = lexical_cast<string>(1);
    const char *argv[] =
    {
        "ssh_cache_tests",
        "--port",
        listenPortAsString.c_str(),
        "--real-backend-host",
        "localhost",
        "--real-backend-port",
        realBackendPortAsString.c_str(),
        "--fake-backend-host",
        "localhost",
        "--fake-backend-port",
        fakeBackendPortAsString.c_str(),
        "--initial-mitm-attacks",
        mitmAttacksAsString.c_str(),
        "--client-expiration",
        "1"
    };
    int argc = sizeof(argv) / sizeof(argv[0]);
    Options options(argc, argv);
    ServerRunner serverRunner(options);

    RandomResponseServer realBackendServer(realBackendPort);
    RandomResponseServer fakeBackendServer(fakeBakendPort);

    list<shared_ptr<string> > fakeIncomingLines, fakeOutgoingLines;
    utils::transferSomeLines("localhost", listenPort, fakeIncomingLines, fakeOutgoingLines);
    sleep(milliseconds(1250));
    utils::transferSomeLines("localhost", listenPort, fakeIncomingLines, fakeOutgoingLines);

    BOOST_REQUIRE(fakeIncomingLines == fakeBackendServer.getOutgoingLines());
    BOOST_REQUIRE(fakeOutgoingLines == fakeBackendServer.getIncomingLines());
    BOOST_REQUIRE(realBackendServer.getOutgoingLines().empty());
    BOOST_REQUIRE(realBackendServer.getIncomingLines().empty());
}


}
}
}
}
