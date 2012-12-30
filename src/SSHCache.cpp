#include "SSHCache.hpp"
#include "Server.hpp"
#include "Options.hpp"
using namespace org::voidland::ssh_cache;

#include <exception>
#include <iostream>
using namespace std;


namespace org
{
namespace voidland
{
namespace ssh_cache
{


int main(int argc, char *argv[])
{
    Options options(argc, argv);
    if (options.isHelp())
    {
        cout << options.getDescription();
        return 0;
    }

    try
    {
        Server server(options);
        server.run();
    }
    catch (const std::exception &e)
    {
        cerr << "Uncaught exception: " << e.what() << endl;
    }
    return 0;
}


}
}
}
