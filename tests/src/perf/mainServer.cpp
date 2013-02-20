#include "EchoServer.hpp"
using namespace org::voidland::ssh_cache::test::performance;

#include "TestUtils.hpp"
using namespace org::voidland::ssh_cache::test;

#include "Server.hpp"
using namespace org::voidland::ssh_cache;

#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>
using namespace boost;
using namespace boost::program_options;

#include <sstream>
#include <string>
using namespace std;


namespace org
{
namespace voidland
{
namespace ssh_cache
{
namespace performance
{


class Options
{
private:
    options_description optionsDescription;
    variables_map vm;

public:
    Options(int argc, const char * const *argv);

    string getDescription(void) const;

    bool isHelp(void) const;
    unsigned short getPort(void) const;
};

static const char *helpOptionName = "help";
static const char *portOptionName = "port";


Options::Options(int argc, const char * const *argv) :
    optionsDescription("ssh_cache options")
{
    this->optionsDescription.add_options()
        (helpOptionName, "help")
        (portOptionName, value<unsigned short>()->default_value(8022), "listen port");

    positional_options_description p;
    p.add("", 0);

    store(command_line_parser(argc, argv).options(this->optionsDescription).positional(p).run(), this->vm);
}

string Options::getDescription(void) const
{
    ostringstream str;
    str << this->optionsDescription;
    return str.str();
}

bool Options::isHelp(void) const
{
    return this->vm.count(helpOptionName) > 0;
}

unsigned short Options::getPort(void) const
{
    return this->vm[portOptionName].as<unsigned short>();
}


}
}
}
}


int main(int argc, char *argv[])
{
    try
    {
        org::voidland::ssh_cache::performance::Options options(argc, argv);
        if (options.isHelp())
        {
            cout << options.getDescription();
            return 0;
        }

        unsigned short echoServerPort = utils::findFreePort();
        EchoServer echoServer(echoServerPort);

        string echoServerPortAsString = lexical_cast<string>(echoServerPort);
        string listenPortAsString = lexical_cast<string>(options.getPort());
        const char *argv[] =
        {
            "ssh_cache_perf",
            "--port",
            listenPortAsString.c_str(),
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

        Options serverOptions(argc, argv);
        Server server(serverOptions);
        server.run();
    }
    catch (const std::exception &e)
    {
        cerr << e.what() << endl;
        return 1;
    }
    return 0;
}
