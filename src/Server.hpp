#ifndef _SSH_CACHE_SERVER_HPP_
#define _SSH_CACHE_SERVER_HPP_

#include "Options.hpp"

#include <boost/system/system_error.hpp>


namespace org
{
namespace voidland
{
namespace ssh_cache
{


using namespace boost::system;


class Server
{
private:
    const Options &options;

public:
    Server(const Options &options);
    void run(void)
        throw (system_error);
};


}
}
}

#endif
