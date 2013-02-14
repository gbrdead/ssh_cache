#include "PerfTests.hpp"

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>

#include <cmath>
#include <cstdio>
#include <iostream>
#include <list>
#include <sstream>
#include <stdexcept>
#include <utility>

extern "C"
{
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
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
                    "ssh", "-t",
                        "-o", "StrictHostKeyChecking=no",
                        "-o", "UserKnownHostsFile=/dev/null",
                        "-p", portAsString.c_str(),
                        userNameAndHost.c_str(),
                0
            };

            int devNullFD = open("/dev/null", O_WRONLY);
            if (devNullFD == -1)
            {
                perror("open /dev/null");
                exit(1);
            }

            dup2(devNullFD, 1);
            dup2(devNullFD, 2);
            close(devNullFD);

            execvp("sshpass", (char * const *)argv);
            exit(1);
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
        if (!this->failure  &&  result != -1  &&  WIFEXITED(status)  &&  WEXITSTATUS(status) == 0)
        {
            this->successfulConnectionsUntilFirstError++;
        }
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

    this->successfulConnectionsUntilFirstError = 0;
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

unsigned PerformanceTest::getSuccessfulConnectionsUntilFirstError(void)
{
    return this->successfulConnectionsUntilFirstError;
}


PerformanceTestsExecutor::PerformanceTestsExecutor(const Options &options) :
    perfTest(options), perfTestForRecovery(options)
{
}

bool PerformanceTestsExecutor::execute(unsigned count)
{
    cout << "Testing with " << count << " simultaneous connections... " << flush;
    bool success = this->perfTest.execute(count);
    if (success)
    {
        cout << "success. The maximum response time is " << this->perfTest.getMaxTime() / 1000000 << " ms.";
    }
    else
    {
        cout << "failure. The count of successful connections before the first failure is " << this->perfTest.getSuccessfulConnectionsUntilFirstError() << ".";
    }
    cout << endl;

    // Give the target a chance to clean up.
    cpu_timer timer;
    while (!this->perfTestForRecovery.execute(1));
    nanosecond_type recoveryTime = timer.elapsed().wall;
    cout << "The recovery time is " << recoveryTime / 1000000 << " ms." << endl;

    if (this->perfTest.getSuccessfulConnectionsUntilFirstError() == 0)
    {
        return this->execute(count);
    }

    return success;
}

pair<unsigned, nanosecond_type> PerformanceTestsExecutor::findGreatestSuccess(unsigned lowCount, unsigned highCount, nanosecond_type lowMaxTime)
{
    if ((highCount - lowCount) <= 1)
    {
        return pair<unsigned, nanosecond_type>(lowCount, lowMaxTime);
    }

    unsigned count = (highCount - lowCount) / 2 + lowCount;
    if (this->execute(count))
    {
        return this->findGreatestSuccess(count, highCount, perfTest.getMaxTime());
    }
    return this->findGreatestSuccess(lowCount, count, lowMaxTime);
}

void PerformanceTestsExecutor::execute(void)
{
    nanosecond_type lowMaxTime;
    unsigned lowCount, highCount;
    lowCount = highCount = 0;
    for (int powerOf10 = 0; ; powerOf10++)
    {
        highCount = (long)pow((float)10, powerOf10);
        if (!this->execute(highCount))
        {
            break;
        }
        lowCount = highCount;
        lowMaxTime = this->perfTest.getMaxTime();
    }
    if (highCount == lowCount)
    {
        throw runtime_error("Not a single successful test execution!");
    }

    pair<unsigned, nanosecond_type> greatestSuccess = this->findGreatestSuccess(lowCount, highCount, lowMaxTime);
    cout << "The greatest success is " << greatestSuccess.first << " simultaneous connections, "
         << "the maximum response time is " << greatestSuccess.second / 1000000 << " ms." << endl;
}


}
}
}
}
}
