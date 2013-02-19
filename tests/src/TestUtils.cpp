#include "SocketUtils.hpp"
#include "TestUtils.hpp"

#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/random.hpp>
#include <boost/scoped_ptr.hpp>
using namespace boost;
using namespace boost::asio;
using namespace boost::asio::ip;
using namespace boost::random;
using namespace boost::system;

#include <ctime>
#include <iostream>
#include <sstream>
#include <stdexcept>


namespace org
{
namespace voidland
{
namespace ssh_cache
{
namespace test
{


static bool isPortFree(unsigned short port)
{
    io_service ioService;
    {
        scoped_ptr<tcp::acceptor> v6Acceptor;
        scoped_ptr<tcp::acceptor> v4Acceptor;

        try
        {
            v6Acceptor.reset(new tcp::acceptor(ioService, tcp::endpoint(tcp::v6(), port)));
        }
        catch (const system_error &e)
        {
        }
        try
        {
            v4Acceptor.reset(new tcp::acceptor(ioService, tcp::endpoint(tcp::v4(), port)));
        }
        catch (const system_error &e)
        {
        }
        return v6Acceptor || v4Acceptor;
    }
}

list<unsigned short> findFreePorts(size_t count)
{
    list<unsigned short> freePorts;

    for (unsigned short port = 49152;
        port <= 65535 && port >= 49152  &&  freePorts.size() < count;
        port++)
    {
        if (isPortFree(port))
        {
            freePorts.push_back(port);
        }
    }
    for (unsigned short port = 1024;
        port < 49152  &&  freePorts.size() < count;
        port++)
    {
        if (isPortFree(port))
        {
            freePorts.push_back(port);
        }
    }
    if (freePorts.size() < count)
    {
        throw runtime_error("Cannot find " + lexical_cast<string>(count) + " free ports.");
    }
    return freePorts;
}

unsigned short findFreePort(void)
{
    return findFreePorts(1).front();
}

void findFreePorts(unsigned short &listenPort, unsigned short &realBackendPort, unsigned short &fakeBakendPort)
{
    list<unsigned short> freePorts = findFreePorts(3);
    listenPort = freePorts.front();
    freePorts.pop_front();
    realBackendPort = freePorts.front();
    freePorts.pop_front();
    fakeBakendPort = freePorts.front();
    freePorts.pop_front();
}

string generateRandomString(void)
{
    static string chars(
        "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "1234567890"
        "!@#$%^&*()"
        "`~-_=+[{]{\\|;:'\",<.>/?");
    mt19937 gen(time(0));
    uniform_int_distribution<size_t> stringLengthDist(1, 255);
    uniform_int_distribution<size_t> charDist(0, chars.length() - 1);

    size_t stringLength = stringLengthDist(gen);
    ostringstream str;
    for (size_t i = 0; i < stringLength; i++)
    {
        str << chars[charDist(gen)];
    }
    return str.str();
}

void transferSomeLines(
    const string &host, unsigned short port,
    list<shared_ptr<string> > &incomingLines, list<shared_ptr<string> > &outgoingLines)
{
    io_service ioService;
    tcp::socket socket(ioService);
    socket_utils::connect(socket, host, lexical_cast<string>(port));

    mt19937 gen(time(0));
    uniform_int_distribution<unsigned> lineCountDist(1, 100);
    unsigned lineCount = lineCountDist(gen);

    asio::streambuf buf;
    for (unsigned i = 0; i < lineCount; i++)
    {
        shared_ptr<string> outgoingLine(new string(generateRandomString()));
        writeLine(socket, *outgoingLine);

        shared_ptr<string> incomingLine(new string(readLine(socket, buf)));

        outgoingLines.push_back(outgoingLine);
        incomingLines.push_back(incomingLine);
    }

    socket_utils::close(socket);
}

bool operator==(const list<shared_ptr<string> > &l1, const list<shared_ptr<string> > &l2)
{
    if (l1.size() != l2.size())
    {
        return false;
    }

    for (list<shared_ptr<string> >::const_iterator i1 = l1.begin(), i2 = l2.begin();
         i1 != l1.end();
         i1++, i2++)
    {
        if (**i1 != **i2)
        {
            return false;
        }
    }

    return true;
}

void writeLine(tcp::socket &socket, const string &line)
    throw (system_error)
{
    string tmpStr(line);
    tmpStr += "\n";
    write(socket, buffer(tmpStr.c_str(), tmpStr.length() * sizeof(string::value_type)));
}

string readLine(tcp::socket &socket, asio::streambuf &buf)
    throw (system_error)
{
    string line;
    istream is(&buf);
    read_until(socket, buf, '\n');
    getline(is, line);
    return line;
}


}
}
}
}
