#include "PerfTests.hpp"
#include "SocketUtils.hpp"
#include "TestUtils.hpp"
using namespace org::voidland::ssh_cache;
using namespace org::voidland::ssh_cache::test;

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/scoped_ptr.hpp>
using namespace boost::system;

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


void PerformanceTest::executeOnce(barrier &b)
{
    b.wait();
    this->execute();
}

void PerformanceTest::fail(void)
{
    if (this->failure)  // Non-synchronized access => this->failure must be volatile.
    {
        return;
    }
    this->cleanupAfterFailure();
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

    return !this->failure;
}

PerformanceTest::PerformanceTest(const Options &options) :
    options(options)
{
}

PerformanceTest::~PerformanceTest(void)
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



RealPerformanceTest::RealPerformanceTest(const Options &options) :
    PerformanceTest(options)
{
}

RealPerformanceTest::~RealPerformanceTest(void)
{
}

void RealPerformanceTest::execute(void)
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

void RealPerformanceTest::cleanupAfterFailure(void)
{
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

bool RealPerformanceTest::execute(unsigned count)
{
    bool retVal = PerformanceTest::execute(count);
    if (!this->childProcesses.empty())
    {
        throw runtime_error("childProcesses is not empty!");
    }
    return retVal;
}



MockPerformanceTest::MockPerformanceTest(const Options &options) :
    PerformanceTest(options)
{
}

MockPerformanceTest::~MockPerformanceTest(void)
{
}

void MockPerformanceTest::execute(void)
{
    shared_ptr<tcp::socket> socket(new tcp::socket(this->ioService));
    cpu_timer timer;

    try
    {
        socket_utils::connect(*socket, this->options.getHost(), lexical_cast<string>(this->options.getPort()));
    }
    catch (const system_error &e)
    {
        this->fail();
        return;
    }

    {
        timer.stop();
        mutex::scoped_lock am(this->bigMutex);
        if (this->failure)
        {
            socket_utils::close(*socket);
            return;
        }
        this->connections.insert(socket);
        timer.resume();
    }

    const static string line("1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012");

    bool thisFailed = false;
    try
    {
        asio::streambuf buf;
        for (unsigned i = 0; i < 11; i++)
        {
            writeLine(*socket, line);
            readLine(*socket, buf);
        }
        socket->shutdown(tcp::socket::shutdown_both);
        socket->close();
    }
    catch (const system_error &e)
    {
        socket_utils::close(*socket);
        thisFailed = true;
    }

    nanosecond_type ns = timer.elapsed().wall;
    {
        mutex::scoped_lock am(this->bigMutex);
        this->connections.erase(socket);
        if (!this->failure  &&  !thisFailed)
        {
            this->successfulConnectionsUntilFirstError++;
        }
    }

    if (thisFailed)
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

void MockPerformanceTest::cleanupAfterFailure(void)
{
    mutex::scoped_lock am(this->bigMutex);
    this->failure = true;

    for (set<shared_ptr<tcp::socket> >::const_iterator i = this->connections.begin(); i != this->connections.end(); i++)
    {
        socket_utils::close(**i);
    }
}

bool MockPerformanceTest::execute(unsigned count)
{
    bool retVal = PerformanceTest::execute(count);
    if (!this->connections.empty())
    {
        throw runtime_error("connections is not empty!");
    }
    return retVal;
}



PerformanceTestsExecutor::PerformanceTestsExecutor(const Options &options)
{
    if (options.isMock())
    {
        this->perfTest.reset(new MockPerformanceTest(options));
        this->perfTestForRecovery.reset(new MockPerformanceTest(options));
    }
    else
    {
        this->perfTest.reset(new RealPerformanceTest(options));
        this->perfTestForRecovery.reset(new RealPerformanceTest(options));
    }
}

bool PerformanceTestsExecutor::execute(unsigned count)
{
    cout << "Testing with " << count << " simultaneous connections... " << flush;
    bool success = this->perfTest->execute(count);
    if (success)
    {
        cout << "success. The maximum response time is " << this->perfTest->getMaxTime() / 1000000 << " ms.";
    }
    else
    {
        cout << "failure. The count of successful connections before the first failure is " << this->perfTest->getSuccessfulConnectionsUntilFirstError() << ".";
    }
    cout << endl;

    // Give the target a chance to clean up.
    cpu_timer timer;
    while (!this->perfTestForRecovery->execute(1));
    nanosecond_type recoveryTime = timer.elapsed().wall;
    cout << "The recovery time is " << recoveryTime / 1000000 << " ms." << endl;

    if (this->perfTest->getSuccessfulConnectionsUntilFirstError() == 0)
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
        return this->findGreatestSuccess(count, highCount, this->perfTest->getMaxTime());
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
        lowMaxTime = this->perfTest->getMaxTime();
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
