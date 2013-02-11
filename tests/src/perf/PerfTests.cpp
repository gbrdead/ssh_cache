#include "PerfTests.hpp"

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
using namespace boost::posix_time;
using namespace boost::this_thread;

#include <cstdio>
#include <iostream>
#include <list>
#include <sstream>
#include <stdexcept>

extern "C"
{
#include <signal.h>
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
    pid_t pid;
    scoped_ptr<cpu_timer> timer;

    {
        mutex::scoped_lock am(this->bigMutex);
        if (this->failure)
        {
            return;
        }

        pid = fork();

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

        timer.reset(new cpu_timer());
        this->childProcesses.insert(pid);
    }

    int status;
    pid_t result;

    do
    {
        result = waitpid(pid, &status, 0);
    }
    while (result != -1  &&  !WIFEXITED(status)  &&  !WIFSIGNALED(status));

    nanosecond_type ns = timer->elapsed().wall;
    {
        mutex::scoped_lock am(this->bigMutex);
        this->childProcesses.erase(pid);
    }

    if (result == -1)
    {
        perror("waitpid");
        this->fail();
        return;
    }
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
    if (this->failure)
    {
        return;
    }

    mutex::scoped_lock am(this->bigMutex);
    this->failure = true;

    for (set<pid_t>::const_iterator i = this->childProcesses.begin(); i != this->childProcesses.end(); i++)
    {
        cerr << "Killing child process " << *i << endl;
        if (kill(*i, SIGKILL) == -1)
        {
            if (errno != ESRCH)
            {
                perror("kill");
            }
        }
    }
}

bool PerformanceTest::execute(unsigned count)
{
    barrier b(count);
    list<shared_ptr<thread> > threads;

    this->maxTime = 0;
    this->failure = false;

    for (unsigned i = 0; i < count; i++)
    {
        shared_ptr<thread> thr(new thread(&PerformanceTest::executeOnce, this, ref(b)));
        threads.push_back(thr);
    }

    for (list<shared_ptr<thread> >::iterator i = threads.begin(); i != threads.end(); i++)
    {
        (*i)->join();
    }

    if (!this->childProcesses.empty())
    {
        throw runtime_error("childProcesses is not empty!");
    }

    // Give the target a chance to clean up.
    sleep(seconds(this->failure ? 10 : 1));

    return !this->failure;
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
