#ifndef _SSH_CACHE_PERF_TESTS_HPP_
#define _SSH_CACHE_PERF_TESTS_HPP_

#include "PerfTestsOptions.hpp"

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
using namespace boost::timer;
using namespace std;


class PerformanceTest
{
private:
    const Options &options;

    mutex bigMutex;
    volatile bool failure;
    set<pid_t> childProcesses;

    unsigned successfulConnectionsUntilFirstError;
    nanosecond_type maxTime;
    mutex maxTimeMutex;


    void execute(void);
    void executeOnce(barrier &b);

    void fail(void);

public:
    PerformanceTest(const Options &options);

    bool execute(unsigned count);
    nanosecond_type getMaxTime(void);
    unsigned getSuccessfulConnectionsUntilFirstError(void);
};


class PerformanceTestsExecutor
{
private:
    PerformanceTest perfTest, perfTestForRecovery;

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
