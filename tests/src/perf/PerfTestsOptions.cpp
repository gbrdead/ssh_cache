#include "PerfTestsOptions.hpp"

#include <sstream>


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


static const char *helpOptionName = "help";
static const char *mockOptionName = "mock";
static const char *hostOptionName = "host";
static const char *portOptionName = "port";
static const char *userNameOptionName = "username";
static const char *passwordOptionName = "password";


Options::Options(int argc, const char * const *argv) :
    optionsDescription("ssh_cache options")
{
    this->optionsDescription.add_options()
        (helpOptionName, "help")
        (mockOptionName, "run against ssh_cache_perf")
        (hostOptionName, value<string>()->default_value("es"), "ssh_cache host")
        (portOptionName, value<unsigned short>()->default_value(8022), "ssh_cache port")
        (userNameOptionName, value<string>()->default_value("gc"), "ssh_cache user name")
        (passwordOptionName, value<string>()->default_value("alabala"), "ssh_cache user password");

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

bool Options::isMock(void) const
{
    return this->vm.count(mockOptionName) > 0;
}

const string &Options::getHost(void) const
{
    return this->vm[hostOptionName].as<string>();
}

unsigned short Options::getPort(void) const
{
    return this->vm[portOptionName].as<unsigned short>();
}

const string &Options::getUserName(void) const
{
    return this->vm[userNameOptionName].as<string>();
}

const string &Options::getPassword(void) const
{
    return this->vm[passwordOptionName].as<string>();
}


}
}
}
}
}
