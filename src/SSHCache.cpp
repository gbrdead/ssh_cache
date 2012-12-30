#include "Server.hpp"
#include "Options.hpp"
using namespace org::voidland::ssh_cache;

#include <boost/program_options.hpp>
using namespace boost::program_options;

#include <exception>
#include <iostream>
#include <string>
using namespace std;


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
