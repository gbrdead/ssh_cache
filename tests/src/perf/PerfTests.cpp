#include "PerfTests.hpp"

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>

#include <cstdio>
#include <list>
#include <sstream>
using namespace std;

extern "C"
{
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
}


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
    pid_t pid = fork();

    if (pid == -1)
    {
        perror("fork");
        this->fail();
        return;
    }

    if (pid == 0)
    {
        string portAsString(lexical_cast<string>(this->options.getPort()));
        string userNameAndHost = this->options.getUserName() + "@" + this->options.getHost();
        const char *const argv[] =
        {
            "sshpass",
                "-p", this->options.getPassword().c_str(),
                "ssh",
                    "-o", "StrictHostKeyChecking=no",
                    "-o", "UserKnownHostsFile=/dev/null",
                    "-p", portAsString.c_str(),
                    userNameAndHost.c_str(),
            0
        };

        execvp("sshpass", (char * const *)argv);
        perror("execvp");
        this->fail();
        return;
    }


    cpu_timer timer;

    int status;
    if (waitpid(pid, &status, 0 /* WNOHANG */) == -1)
    {
        perror("waitpid");
        this->fail();
        return;
    }

    nanosecond_type ns = timer.elapsed().wall;

    if (!WIFEXITED(status)  ||  WEXITSTATUS(status) != 0)
    {
        this->fail();
        return;
    }

    {
        mutex::scoped_lock am(this->maxTimeMutex);
        if (this->maxTime < ns)
        {
            this->maxTime = ns;
        }
    }
}

void PerformanceTest::executeOnce(barrier &b)
{
    b.wait();
    this->execute();
}

void PerformanceTest::fail(void)
{
    this->success = false;
    // TODO: force all of the running threads to abort.
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
