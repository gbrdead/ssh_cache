#ifndef _SSH_CACHE_PERF_TESTS_HPP_
#define _SSH_CACHE_PERF_TESTS_HPP_

#include "PerfTestsOptions.hpp"

#include <boost/thread.hpp>
#include <boost/timer/timer.hpp>


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


class PerformanceTest
{
private:
    const Options &options;

    nanosecond_type maxTime;
    bool success;
    mutex maxTimeMutex;

    void execute(void);
    void executeOnce(barrier &b);
    void fail(void);

public:
    PerformanceTest(const Options &options);

    bool execute(unsigned count);
    nanosecond_type getMaxTime(void);
};


}
}
}
}
}

#endif
