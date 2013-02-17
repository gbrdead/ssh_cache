#ifndef _SSH_CACHE_RANDOM_RESPONSE_SERVER_HPP_
#define _SSH_CACHE_RANDOM_RESPONSE_SERVER_HPP_

#include "TestServer.hpp"

#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

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


class RandomResponseServer :
    public TestServer
{
private:
    list<shared_ptr<string> > incomingLines;
    mutex incomingLinesMutex;
    list<shared_ptr<string> > outgoingLines;
    mutex outgoingLinesMutex;

protected:
    virtual void processIncomingBuffer(tcp::socket &socket, asio::streambuf &buf);

public:
    RandomResponseServer(unsigned short port);
    virtual ~RandomResponseServer(void);

    list<shared_ptr<string> > getIncomingLines(void);
    list<shared_ptr<string> > getOutgoingLines(void);
};


}
}
}
}

#endif
