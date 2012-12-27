#include "Client.hpp"
#include "Constants.hpp"

#include <boost/bind.hpp>
using namespace boost::posix_time;

#include <utility>
#include <iostream>


namespace org
{
namespace voidland
{
namespace ssh_cache
{


Client::Client(ClientService &clientService, const address &id, io_service &ioService)
    : clientService(clientService),
      id(id),
      mitmAttacksCount(0),
      expirationTimer(ioService)
{
}

Client::~Client(void)
{
    mutex::scoped_lock am(this->clientService.allClientsMutex);

    map<address, weak_ptr<Client> >::iterator f = this->clientService.allClients.find(this->id);
    if (f != this->clientService.allClients.end())  // Should always be true.
    {
        if (f->second.expired())    // The pointer mignt not be expired if a new client with this->id has been created just before this d'tor is called.
        {
            this->clientService.allClients.erase(f);
        }
    }

    cout << "Client destroyed: " << this->id << endl;
}

void Client::connected(void)
{
    shared_ptr<Client> thisAsSharedPtr = this->clientService.getClient(this->id);
    {
        mutex::scoped_lock am(this->clientService.activeClientsMutex);
        this->clientService.activeClients.insert(thisAsSharedPtr);  // One instance of thisAsSharedPtr for each (future) call to expired().
    }

    this->expirationTimer.expires_from_now(seconds(CLIENT_EXPIRATION_IN_S));
    this->expirationTimer.async_wait(bind(&Client::expired, this, placeholders::error));
}

void Client::expired(const error_code &err)
{
    shared_ptr<Client> thisAsSharedPtr = this->clientService.getClient(this->id);
    mutex::scoped_lock am(this->clientService.activeClientsMutex);

    multiset<shared_ptr<Client> >::iterator f = this->clientService.activeClients.find(thisAsSharedPtr);
    if (f != this->clientService.activeClients.end())   // Should always be true, i.e. unnecessary.
    {
        this->clientService.activeClients.erase(f); // Erases only one of the instances of thisAsSharedPtr.
    }
}

unsigned Client::getMitmAttacksCount(void) const
{
    return this->mitmAttacksCount;
}

void Client::addMitmAttack(void)
{
    this->mitmAttacksCount++;
}


ClientService::ClientService(io_service &ioService) :
    ioService(ioService)
{
}

shared_ptr<Client> ClientService::getClient(const address &id)
{
    mutex::scoped_lock am(this->allClientsMutex);

    shared_ptr<Client> retVal;

    map<address, weak_ptr<Client> >::iterator f = this->allClients.find(id);
    if (f != this->allClients.end())
    {
        retVal = f->second.lock();
    }
    if (!retVal)
    {
        retVal.reset(new Client(*this, id, this->ioService));
        if (f != this->allClients.end())
        {
            f->second = retVal;
        }
        else
        {
            this->allClients.insert(make_pair(id, retVal));
        }
    }

    return retVal;
}

}
}
}
