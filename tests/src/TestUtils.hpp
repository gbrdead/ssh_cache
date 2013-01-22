#ifndef _SSH_CACHE_TEST_UTILS_HPP_
#define _SSH_CACHE_TEST_UTILS_HPP_

#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>

#include <list>
#include <string>


namespace org
{
namespace voidland
{
namespace ssh_cache
{
namespace test
{


using namespace boost;
using namespace boost::asio::ip;
using namespace std;


list<unsigned short> findFreePorts(size_t count);
unsigned short findFreePort(void);
void findFreePorts(unsigned short &listenPort, unsigned short &realBackendPort, unsigned short &fakeBakendPort);

string generateRandomString(void);

void transferSomeLines(
    const string &host, unsigned short port,
    list<shared_ptr<string> > &incomingLines, list<shared_ptr<string> > &outgoingLines);

bool operator==(const list<shared_ptr<string> > &l1, const list<shared_ptr<string> > &l2);

void writeLine(tcp::socket &socket, const string &line);

}
}
}
}

#endif
