#include "Constants.hpp"

#include <boost/asio.hpp>
using namespace boost::asio;
using namespace boost::asio::ip;

namespace org
{
namespace voidland
{
namespace ssh_cache
{

int main(void)
{
    io_service ioService;
    tcp::acceptor acceptor(ioService, tcp::endpoint(tcp::v6(), PORT));
    tcp::socket socket(ioService);
    acceptor.accept(socket);
    return 0;
}



}
}
}


int main(void)
{
    return org::voidland::ssh_cache::main();
}
