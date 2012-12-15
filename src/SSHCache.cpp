#include "ClientConnection.hpp"
#include "Constants.hpp"

#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
using namespace boost::asio;
using namespace boost::asio::ip;
using namespace boost::system;
using namespace boost;

#include <iostream>
using namespace std;


namespace org
{
namespace voidland
{
namespace ssh_cache
{


static void runAcceptThread(io_service *ioService, tcp::acceptor *acceptor)
{
    while (true)
    {
        tcp::socket *socket = new tcp::socket(*ioService);
        acceptor->accept(*socket);
        pair<shared_ptr<thread>, weak_ptr<ClientConnection> > clientConnPair = ClientConnection::start(socket);
    }
}

int main(void)
{
    io_service ioService;

    tcp::acceptor *v6Acceptor = 0;
    tcp::acceptor *v4Acceptor = 0;
    try
    {
        v6Acceptor = new tcp::acceptor(ioService, tcp::endpoint(tcp::v6(), PORT));
    }
    catch (system_error &e)
    {
        cerr << "Cannot create TCP server socket on port " << PORT << ", protocol IPv6: " << e.what() << endl;
    }
    try
    {
        bool v4AlreadyBound = false;
        if (v6Acceptor != NULL)
        {
           v6_only v6OnlyOption;
            v6Acceptor->get_option(v6OnlyOption);
            v4AlreadyBound = !v6OnlyOption.value();
        }
        if (!v4AlreadyBound)
        {
            v4Acceptor = new tcp::acceptor(ioService, tcp::endpoint(tcp::v4(), PORT));
        }
    }
    catch (system_error &e)
    {
        cerr << "Cannot create TCP server socket on port " << PORT << ", protocol IPv4: " << e.what() << endl;
    }

    thread *v4Thread = 0;
    thread *v6Thread = 0;
    if (v4Acceptor)
    {
        v4Thread = new thread(runAcceptThread, &ioService, v4Acceptor);
    }
    if (v6Acceptor)
    {
        v6Thread = new thread(runAcceptThread, &ioService, v6Acceptor);
    }
    if (v4Thread)
    {
        v4Thread->join();
        delete v4Thread;
        delete v4Acceptor;
    }
    if (v6Thread)
    {
        v6Thread->join();
        delete v6Thread;
        delete v6Acceptor;
    }

    return 0;
}


}
}
}

int main(void)
{
    return org::voidland::ssh_cache::main();
}
