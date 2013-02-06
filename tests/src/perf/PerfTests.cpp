#include "PerfTests.hpp"

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>

#include <cstdlib>
#include <cstdio>
#include <list>
#include <sstream>
using namespace std;


namespace org
{
namespace voidland
{
namespace ssh_cache
{
namespace test
{
namespace performance
{


void PerformanceTest::execute(void)
{
    ostringstream os;
    os << "sshpass -p \"" << this->options.getPassword() << "\" ";
    os << "ssh -o StrictHostKeyChecking=no -o UserKnownHostsFile=/dev/null ";
    os << "-p " << this->options.getPort() << " " << this->options.getUserName() << "@" << this->options.getHost();
    string command = os.str();

    cpu_timer timer;
    int error = std::system(command.c_str());
    nanosecond_type ns = timer.elapsed().wall;

    if (error == -1)
    {
        perror("");
    }

    {
        mutex::scoped_lock am(this->maxTimeMutex);
        if (this->maxTime < ns)
        {
            this->maxTime = ns;
        }
        if (error != 0)
        {
            this->success = false;
        }
    }
}

void PerformanceTest::executeOnce(barrier &b)
{
    b.wait();
    this->execute();
}

bool PerformanceTest::execute(unsigned count)
{
    barrier b(count);
    list<shared_ptr<thread> > threads;

    this->maxTime = 0;
    this->success = true;

    for (unsigned i = 0; i < count; i++)
    {
        shared_ptr<thread> thr(new thread(&PerformanceTest::executeOnce, this, ref(b)));
        threads.push_back(thr);
    }

    for (list<shared_ptr<thread> >::iterator i = threads.begin(); i != threads.end(); i++)
    {
        (*i)->join();
    }

    return this->success;
}

PerformanceTest::PerformanceTest(const Options &options) :
    options(options)
{
}

nanosecond_type PerformanceTest::getMaxTime(void)
{
    return this->maxTime;
}


}
}
}
}
}
