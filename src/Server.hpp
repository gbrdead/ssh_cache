#ifndef _SSH_CACHE_SERVER_HPP_
#define _SSH_CACHE_SERVER_HPP_

#include "Options.hpp"


namespace org
{
namespace voidland
{
namespace ssh_cache
{


class Server
{
private:
    const Options &options;

public:
    Server(const Options &options);
    void run(void);
};


}
}
}

#endif
