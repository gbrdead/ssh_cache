#ifndef _SSH_CACHE_PERF_TESTS_HPP_
#define _SSH_CACHE_PERF_TESTS_HPP_

#include "PerfTestsOptions.hpp"

#include <boost/asio.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/timer/timer.hpp>

#include <set>

extern "C"
{
#include <unistd.h>
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


using namespace boost;
using namespace boost::asio;
using namespace boost::asio::ip;
using namespace boost::timer;
using namespace std;


class PerformanceTest
{
private:
    void executeOnce(barrier &b);

protected:
    const Options &options;
    volatile bool failure;
    unsigned successfulConnectionsUntilFirstError;
    nanosecond_type maxTime;
    mutex maxTimeMutex;

    virtual void execute(void) = 0;
    virtual void cleanupAfterFailure(void) = 0;
    void fail(void);

public:
    PerformanceTest(const Options &options);
    virtual ~PerformanceTest(void);

    virtual bool execute(unsigned count);
    nanosecond_type getMaxTime(void);
    unsigned getSuccessfulConnectionsUntilFirstError(void);
};


class RealPerformanceTest :
    public PerformanceTest
{
private:
    mutex bigMutex;
    set<pid_t> childProcesses;

protected:
    virtual void execute(void);
    virtual void cleanupAfterFailure(void);

public:
    RealPerformanceTest(const Options &options);
    virtual ~RealPerformanceTest(void);

    virtual bool execute(unsigned count);
};


class MockPerformanceTest :
    public PerformanceTest
{
private:
    io_service ioService;
    mutex bigMutex;
    set<shared_ptr<tcp::socket> > connections;

protected:
    virtual void execute(void);
    virtual void cleanupAfterFailure(void);

public:
    MockPerformanceTest(const Options &options);
    virtual ~MockPerformanceTest(void);

    virtual bool execute(unsigned count);
};


class PerformanceTestsExecutor
{
private:
    scoped_ptr<PerformanceTest> perfTest, perfTestForRecovery;

    bool execute(unsigned count);
    pair<unsigned, nanosecond_type> findGreatestSuccess(unsigned lowCount, unsigned highCount, nanosecond_type lowMaxTime);

public:
    PerformanceTestsExecutor(const Options &options);
    void execute(void);
};


}
}
}
}
}

#endif
