#ifndef _SSH_CACHE_TEST_SERVER_HPP_
#define _SSH_CACHE_TEST_SERVER_HPP_

#include <boost/asio.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>


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


class TestServer
{
private:
    io_service ioService;
    scoped_ptr<thread> ioServiceRunThread;
    scoped_ptr<tcp::acceptor> v6Acceptor;
    scoped_ptr<tcp::acceptor> v4Acceptor;

    void runIOServiceThread(void);

    void lineReadHandler(const error_code &err, size_t size, shared_ptr<tcp::socket> socket, shared_ptr<asio::streambuf> buf);
    void asyncLineReader(shared_ptr<tcp::socket> socket, shared_ptr<asio::streambuf> buf);
    void acceptHandler(const error_code &err, shared_ptr<tcp::socket> socket, tcp::acceptor &acceptor);
    void asyncAcceptor(tcp::acceptor &acceptor);

protected:
    virtual void processIncomingBuffer(tcp::socket &socket, asio::streambuf &buf) = 0;

public:
    TestServer(unsigned short port);
    virtual ~TestServer(void);
};


}
}
}
}

#endif
