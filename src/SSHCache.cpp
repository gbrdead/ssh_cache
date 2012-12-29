#include "Server.hpp"
using namespace org::voidland::ssh_cache;

#include <boost/program_options.hpp>
using namespace boost::program_options;

#include <exception>
#include <iostream>
#include <string>
using namespace std;


int main(int argc, char *argv[])
{
    options_description optionsDescrition("ssh_cache options");
    optionsDescrition.add_options()
        ("help", "help")
        ("port", value<string>()->default_value("8022"), "listen port")
        ("real-backend-host", value<string>()->default_value("localhost"), "real backend host")
        ("real-backend-port", value<string>()->default_value("8023"), "real backend port")
        ("fake-backend-host", value<string>()->default_value("localhost"), "fake backend host")
        ("fake-backend-port", value<string>()->default_value("8024"), "fake backend port")
        ("initial-mitm-attacks", value<unsigned>()->default_value(1), "initial man-in-the-middle attacks count")
        ("client-expiration", value<long>()->default_value(86400), "client expiration in seconds");

    variables_map vm;
    store(parse_command_line(argc, argv, optionsDescrition), vm);
    notify(vm);

    if (vm.count("help"))
    {
        cout << optionsDescrition;
        return 0;
    }

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
