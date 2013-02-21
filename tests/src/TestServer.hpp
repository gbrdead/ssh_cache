#ifndef _SSH_CACHE_TEST_SERVER_HPP_
#define _SSH_CACHE_TEST_SERVER_HPP_

#include <boost/asio.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

#include <list>


namespace org
{
namespace voidland
{
namespace ssh_cache
{
namespace test
{


using namespace boost;
using namespace boost::asio;
using namespace boost::asio::ip;
using namespace boost::system;
using namespace std;


class TestServer
{
private:
    io_service ioService;
    list<shared_ptr<thread> > asyncThreads;

    scoped_ptr<tcp::acceptor> v6Acceptor;
    scoped_ptr<tcp::acceptor> v4Acceptor;

    void lineReadHandler(const error_code &err, size_t size, shared_ptr<tcp::socket> socket, shared_ptr<asio::streambuf> buf);
    void asyncLineReader(shared_ptr<tcp::socket> socket, shared_ptr<asio::streambuf> buf);
    void acceptHandler(const error_code &err, shared_ptr<tcp::socket> socket, tcp::acceptor &acceptor);
    void asyncAcceptor(tcp::acceptor &acceptor);

protected:
    virtual void processIncomingBuffer(tcp::socket &socket, asio::streambuf &buf) = 0;

public:
    TestServer(unsigned short port, unsigned asyncThreadCount = 1);
    virtual ~TestServer(void);
};


}
}
}
}

#endif
