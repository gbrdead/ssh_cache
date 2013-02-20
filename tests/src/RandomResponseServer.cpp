#include "RandomResponseServer.hpp"
#include "TestUtils.hpp"

#include <iostream>


namespace org
{
namespace voidland
{
namespace ssh_cache
{
namespace test
{


void RandomResponseServer::processIncomingBuffer(tcp::socket &socket, asio::streambuf &buf)
{
    istream is(&buf);
    shared_ptr<string> line(new string);
    getline(is, *line);
    {
        mutex::scoped_lock am(this->incomingLinesMutex);
        this->incomingLines.push_back(line);
    }

    shared_ptr<string> responseLine(new string(utils::generateRandomString()));
    {
        mutex::scoped_lock am(this->outgoingLinesMutex);
        this->outgoingLines.push_back(responseLine);

        utils::writeLine(socket, *responseLine);
    }
}

RandomResponseServer::RandomResponseServer(unsigned short port) :
    TestServer(port)
{
}

RandomResponseServer::~RandomResponseServer(void)
{
}

list<shared_ptr<string> > RandomResponseServer::getIncomingLines(void)
{
    mutex::scoped_lock am(this->incomingLinesMutex);
    return this->incomingLines;
}

list<shared_ptr<string> > RandomResponseServer::getOutgoingLines(void)
{
    mutex::scoped_lock am(this->outgoingLinesMutex);
    return this->outgoingLines;
}


}
}
}
}
