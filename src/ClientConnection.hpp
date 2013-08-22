#ifndef _SSH_CACHE_CLIENT_CONNECTION_HPP_
#define _SSH_CACHE_CLIENT_CONNECTION_HPP_

#include "Client.hpp"
#include "Options.hpp"

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/scoped_array.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

#include <cstddef>


namespace org
{
namespace voidland
{
namespace ssh_cache
{


using namespace boost;
using namespace boost::asio::ip;
using namespace boost::system;
using namespace std;


class ClientConnection
{
private:
    static const size_t BUF_SIZE = 10240;

    const Options &options;
    ClientService &clientService;

    shared_ptr<tcp::socket> clientSocket;
    shared_ptr<tcp::socket> backendSocket;

    io_service::strand clientStrand;
    io_service::strand backendStrand;

    shared_ptr<ClientConnection> thisSendingPtr;
    shared_ptr<ClientConnection> thisReceivingPtr;

    scoped_ptr<thread> sendingThread;
    scoped_ptr<thread> receivingThread;

    scoped_array<char> sendingBuf;
    scoped_array<char> receivingBuf;


    ClientConnection(const Options &options, ClientService &clientService, const shared_ptr<tcp::socket> &socket)
        throw (system_error);
    weak_ptr<ClientConnection> start(void);

    static void closeSocket(const shared_ptr<tcp::socket> &socket)
        throw();
    void sendingDone(void)
        throw();
    void receivingDone(void)
        throw();
    static bool isReceiveError(const error_code &receiveError);
    static bool isSendError(const error_code &sendError);

    void syncTransfer(tcp::socket &sourceSocket, tcp::socket &targetSocket)
        throw();
    void syncSend(void);
    void syncReceive(void);
    weak_ptr<ClientConnection> syncStart(void);

    void asyncReceiveFromBackend(const error_code &sendError, size_t size);
    void asyncSendToBackend(const error_code &receiveError, size_t size);
    void asyncReceiveFromClient(const error_code &sendError, size_t size);
    void asyncSendToClient(const error_code &receiveError, size_t size);
    weak_ptr<ClientConnection> asyncStart(void);

public:
    static weak_ptr<ClientConnection> createAndStart(const Options &options, ClientService &clientService, const shared_ptr<tcp::socket> &socket)
        throw (system_error, thread_resource_error);
    void join(void);
};


}
}
}

#endif
