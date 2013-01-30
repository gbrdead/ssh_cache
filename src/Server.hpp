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


class ServerInternal;

class Server
{
private:
    ServerInternal *impl;

public:
    Server(const Options &options);
    ~Server(void);

    void run(void)
        throw (system_error);
    void ensureRunning(void);
};


}
}
}

#endif
