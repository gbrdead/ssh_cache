#include "Options.hpp"

#include <sstream>


namespace org
{
namespace voidland
{
namespace ssh_cache
{


static const char *helpOptionName = "help";
static const char *portOptionName = "port";
static const char *realBackendHostOptionName = "real-backend-host";
static const char *realBackendPortOptionName = "real-backend-port";
static const char *fakeBackendHostOptionName = "fake-backend-host";
static const char *fakeBackendPortOptionName = "fake-backend-port";
static const char *initialMitmAttacksOptionName = "initial-mitm-attacks";
static const char *clientExpirationOptionName = "client-expiration";
static const char *asyncOptionName = "async";
static const char *asyncThreadCountOptionName = "async-threads";


Options::Options(int argc, const char * const *argv) :
    optionsDescription("ssh_cache options")
{
    this->optionsDescription.add_options()
        (helpOptionName, "help")
        (portOptionName, value<unsigned short>()->default_value(8022), "listen port")
        (realBackendHostOptionName, value<string>()->default_value("localhost"), "real backend host")
        (realBackendPortOptionName, value<string>()->default_value("8023"), "real backend port")
        (fakeBackendHostOptionName, value<string>()->default_value("localhost"), "fake backend host")
        (fakeBackendPortOptionName, value<string>()->default_value("8024"), "fake backend port")
        (initialMitmAttacksOptionName, value<unsigned>()->default_value(1), "initial man-in-the-middle attacks count")
        (clientExpirationOptionName, value<long>()->default_value(3600), "client expiration in seconds")
        (asyncOptionName, "use asynchronous I/O")
        (asyncThreadCountOptionName, value<unsigned>()->default_value(1), "asynchronous I/O thread count");

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

const string &Options::getRealBackendHost(void) const
{
    return this->vm[realBackendHostOptionName].as<string>();
}

const string &Options::getRealBackendPort(void) const
{
    return this->vm[realBackendPortOptionName].as<string>();
}

const string &Options::getFakeBackendHost(void) const
{
    return this->vm[fakeBackendHostOptionName].as<string>();
}

const string &Options::getFakeBackendPort(void) const
{
    return this->vm[fakeBackendPortOptionName].as<string>();
}

unsigned Options::getInitialMitmAttacks(void) const
{
    return this->vm[initialMitmAttacksOptionName].as<unsigned>();
}

long Options::getClientExpirationInS(void) const
{
    return this->vm[clientExpirationOptionName].as<long>();
}

bool Options::isAsync(void) const
{
    return this->vm.count(asyncOptionName) > 0;
}

unsigned Options::getAsyncThreadCount(void) const
{
    return this->vm[asyncThreadCountOptionName].as<unsigned>();
}


}
}
}
