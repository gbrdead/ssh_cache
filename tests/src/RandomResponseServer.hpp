#ifndef _SSH_CACHE_RANDOM_RESPONSE_SERVER_HPP_
#define _SSH_CACHE_RANDOM_RESPONSE_SERVER_HPP_

#include <boost/asio.hpp>
#include <boost/scoped_ptr.hpp>
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
using namespace boost::asio;
using namespace boost::asio::ip;
using namespace boost::system;
using namespace std;


class RandomResponseServer
{
private:
    io_service ioService;
    scoped_ptr<thread> ioServiceRunThread;
    scoped_ptr<tcp::acceptor> v6Acceptor;
    scoped_ptr<tcp::acceptor> v4Acceptor;

    list<shared_ptr<string> > incomingLines;
    mutex incomingLinesMutex;
    list<shared_ptr<string> > outgoingLines;
    mutex outgoingLinesMutex;

    void runIOServiceThread(void);

    void lineReadHandler(const error_code &err, size_t size, shared_ptr<tcp::socket> socket, shared_ptr<asio::streambuf> buf);
    void asyncLineReader(shared_ptr<tcp::socket> socket, shared_ptr<asio::streambuf> buf);
    void acceptHandler(const error_code &err, shared_ptr<tcp::socket> socket, tcp::acceptor &acceptor);
    void asyncAcceptor(tcp::acceptor &acceptor);

public:
    RandomResponseServer(unsigned short port);
    ~RandomResponseServer(void);

    list<shared_ptr<string> > getIncomingLines(void);
    list<shared_ptr<string> > getOutgoingLines(void);
};


}
}
}
}

#endif
