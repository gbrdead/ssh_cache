#include "ClientConnection.hpp"

#include <iostream>
using namespace std;


namespace org
{
namespace voidland
{
namespace ssh_cache
{


ClientConnection::ClientConnection(tcp::socket *socket) :
    socket(socket)
{
}

void ClientConnection::run(void)
{
    cout << "Client communication starting." << endl;
}

ClientConnection::~ClientConnection(void)
{
    cout << "Client communication ended." << endl;
}


void ClientConnection::runThread(shared_ptr<ClientConnection> clientConn)
{
    clientConn->run();
}

pair<shared_ptr<thread>, weak_ptr<ClientConnection> > ClientConnection::start(tcp::socket *socket)
{
    shared_ptr<ClientConnection> clientConn(new ClientConnection(socket));
    shared_ptr<thread> thr(new thread(runThread, clientConn));
    return pair<shared_ptr<thread>, weak_ptr<ClientConnection> >(thr, clientConn);
}


}
}
}
