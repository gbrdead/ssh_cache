#include "Options.hpp"

#include <sstream>


namespace org
{
namespace voidland
{
namespace ssh_cache
{


const char *helpOptionName = "help";
const char *portOptionName = "port";
const char *realBackendHostOptionName = "real-backend-host";
const char *realBackendPortOptionName = "real-backend-port";
const char *fakeBackendHostOptionName = "fake-backend-host";
const char *fakeBackendPortOptionName = "fake-backend-port";
const char *initialMitmAttacksOptionName = "initial-mitm-attacks";
const char *clientExpirationOptionName = "client-expiration";


Options::Options(int argc, char *argv[]) :
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
        (clientExpirationOptionName, value<long>()->default_value(86400), "client expiration in seconds");
    store(parse_command_line(argc, argv, this->optionsDescription), this->vm);
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


}
}
}
