#ifndef _SSH_CACHE_OPTIONS_HPP_
#define _SSH_CACHE_OPTIONS_HPP_

#include <boost/program_options.hpp>

#include <string>


namespace org
{
namespace voidland
{
namespace ssh_cache
{


using namespace boost::program_options;
using namespace std;


class Options
{
private:
    options_description optionsDescription;
    variables_map vm;

public:
    Options(int argc, char *argv[]);

    string getDescription(void) const;

    bool isHelp(void) const;
    unsigned short getPort(void) const;
    const string &getRealBackendHost(void) const;
    const string &getRealBackendPort(void) const;
    const string &getFakeBackendHost(void) const;
    const string &getFakeBackendPort(void) const;
    unsigned getInitialMitmAttacks(void) const;
    long getClientExpirationInS(void) const;
};


}
}
}

#endif
