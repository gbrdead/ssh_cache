#include "Constants.hpp"

#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
using namespace boost::asio;
using namespace boost::asio::ip;
using namespace boost::system;
using namespace boost;

#include <cstdlib>
#include <iostream>
using namespace std;


namespace org
{
namespace voidland
{
namespace ssh_cache
{

static void acceptThread(io_service *ioService, tcp::acceptor *acceptor)
{
    while (true)
    {
        tcp::socket socket(*ioService);
        acceptor->accept(socket);

    }
}

int main(void)
{
    io_service ioService;

    tcp::acceptor *v6Acceptor = NULL;
    tcp::acceptor *v4Acceptor = NULL;
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

    if (v4Acceptor != NULL)
    {

    }

    thread *v4Thread = NULL;
    thread *v6Thread = NULL;
    if (v4Acceptor != NULL)
    {
        v4Thread = new thread(acceptThread, &ioService, v4Acceptor);
    }
    if (v6Acceptor != NULL)
    {
        v6Thread = new thread(acceptThread, &ioService, v6Acceptor);
    }

    if (v4Thread != NULL)
    {
        v4Thread->join();
        delete v4Thread;
        delete v4Acceptor;
    }

    if (v6Thread != NULL)
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
