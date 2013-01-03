#ifndef _SSH_CACHE_TEST_UTILS_HPP_
#define _SSH_CACHE_TEST_UTILS_HPP_

#include <list>


namespace org
{
namespace voidland
{
namespace ssh_cache
{
namespace test
{


using namespace std;


list<unsigned short> findFreePorts(size_t count);
unsigned short findFreePort(void);
void findFreePorts(unsigned short &listenPort, unsigned short &realBackendPort, unsigned short &fakeBakendPort);


}
}
}
}

#endif
