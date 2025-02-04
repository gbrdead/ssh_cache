#ifndef _SSH_CACHE_CLIENT_HPP_
#define _SSH_CACHE_CLIENT_HPP_

#include "Options.hpp"

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/utility.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

#include <map>
#include <set>


namespace org
{
namespace voidland
{
namespace ssh_cache
{


using namespace boost;
using namespace boost::asio;
using namespace boost::asio::ip;
using namespace boost::system;

using namespace std;


class ClientService;

class Client :
    private boost::noncopyable
{
    friend class ClientService;

private:
    const Options &options;
    ClientService &clientService;
    address id;
    unsigned mitmAttacksCount;
    deadline_timer expirationTimer;
    mutex expirationTimerMutex;

    Client(const Options &options, ClientService &clientService, const address &id, io_service &ioService);

    void expired(const error_code &err);

public:
    ~Client(void);

    void connected(void);
    unsigned getMitmAttacksCount(void) const;
    void addMitmAttack(void);
};


class ClientService :
    private boost::noncopyable
{
    friend class Client;

private:
    const Options &options;
    io_service &ioService;

    map<address, weak_ptr<Client> > allClients;
    mutex allClientsMutex;

    multiset<shared_ptr<Client> > activeClients;
    mutex activeClientsMutex;

public:
    ClientService(const Options &options, io_service &ioService);

    shared_ptr<Client> getClient(const address &id);
};


}
}
}

#endif
