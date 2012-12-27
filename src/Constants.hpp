#ifndef _SSH_CACHE_CONSTANTS_HPP_
#define _SSH_CACHE_CONSTANTS_HPP_

#include <string>


namespace org
{
namespace voidland
{
namespace ssh_cache
{


using namespace std;


const unsigned short PROXY_PORT = 8022;

const string REAL_BACKEND_HOST = "es";
const unsigned short REAL_BACKEND_PORT = 8023;
const string FAKE_BACKEND_HOST = "es";
const unsigned short FAKE_BACKEND_PORT = 8024;

const unsigned INITIAL_MITM_ATTACKS_COUNT = 1;

const long CLIENT_EXPIRATION_IN_S = 5;    // 86400


}
}
}

#endif
