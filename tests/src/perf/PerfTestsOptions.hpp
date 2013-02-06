#ifndef _SSH_CACHE_PERF_TESTS_OPTIONS_HPP_
#define _SSH_CACHE_PERF_TESTS_OPTIONS_HPP_

#include <boost/program_options.hpp>

#include <string>


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


using namespace boost::program_options;
using namespace std;


class Options
{
private:
    options_description optionsDescription;
    variables_map vm;

public:
    Options(int argc, const char * const *argv);

    string getDescription(void) const;

    bool isHelp(void) const;
    const string &getHost(void) const;
    unsigned short getPort(void) const;
    const string &getUserName(void) const;
    const string &getPassword(void) const;
};


}
}
}
}
}

#endif
