#include "Server.hpp"
using namespace org::voidland::ssh_cache;

#include <exception>
#include <iostream>
using namespace std;


int main(void)
{
    try
    {
        Server server;
        server.run();
    }
    catch (const std::exception &e)
    {
        cerr << "Uncaught exception: " << e.what() << endl;
    }
    return 0;
}
