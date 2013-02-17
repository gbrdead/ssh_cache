#include "EchoServer.hpp"
using namespace org::voidland::ssh_cache::test::performance;

#include "TestUtils.hpp"
using namespace org::voidland::ssh_cache::test;

#include "Server.hpp"
using namespace org::voidland::ssh_cache;

#include <boost/lexical_cast.hpp>
using namespace boost;

#include <string>
using namespace std;


int main(void)
{
    try
    {
        unsigned short echoServerPort = findFreePort();
        EchoServer echoServer(echoServerPort);

        string echoServerPortAsString = lexical_cast<string>(echoServerPort);
        const char *argv[] =
        {
            "ssh_cache_perf",
            "--real-backend-host",
            "localhost",
            "--real-backend-port",
            echoServerPortAsString.c_str(),
            "--fake-backend-host",
            "localhost",
            "--fake-backend-port",
            echoServerPortAsString.c_str(),
        };
        int argc = sizeof(argv) / sizeof(argv[0]);

        Options options(argc, argv);
        Server server(options);
        server.run();
    }
    catch (const std::exception &e)
    {
        cerr << e.what() << endl;
        return 1;
    }
    return 0;
}
