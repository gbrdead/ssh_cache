#include "Server.hpp"
#include "Options.hpp"
using namespace org::voidland::ssh_cache;

#include <exception>
#include <iostream>
using namespace std;


int main(int argc, char *argv[])
{
    try
    {
        Options options(argc, argv);
        if (options.isHelp())
        {
            cout << options.getDescription();
            return 0;
        }

        Server server(options);
        server.run();
    }
    catch (const std::exception &e)
    {
        cerr << e.what() << endl;
        return 1;
    }
    return 0;
}
