#include "PerfTests.hpp"
#include "PerfTestsOptions.hpp"
using namespace org::voidland::ssh_cache::test::performance;

#include <cmath>
#include <iostream>
#include <utility>
using namespace std;


pair<unsigned, nanosecond_type> findGreatestSuccess(PerformanceTest &perfTest, unsigned lowCount, unsigned highCount, nanosecond_type lowMaxTime)
{
    if ((highCount - lowCount) <= 1)
    {
        return pair<unsigned, nanosecond_type>(lowCount, lowMaxTime);
    }

    unsigned count = (highCount - lowCount) / 2 + lowCount;
    if (perfTest.execute(count))
    {
        return findGreatestSuccess(perfTest, count, highCount, perfTest.getMaxTime());
    }
    return findGreatestSuccess(perfTest, lowCount, count, lowMaxTime);
}

int main(int argc, char *argv[])
{
    try
    {
        Options options(argc, argv);
        if (options.isHelp())
        {
            cout << options.getDescription();
            return 0;
        }

        PerformanceTest perfTest(options);

        nanosecond_type lowMaxTime;
        unsigned lowCount, highCount;
        lowCount = highCount = 0;
        for (int powerOf10 = 0; ; powerOf10++)
        {
            highCount = (long)pow((float)10, powerOf10);
            if (!perfTest.execute(highCount))
            {
                break;
            }
            lowCount = highCount;
            lowMaxTime = perfTest.getMaxTime();
        }
        if (highCount == lowCount)
        {
            throw runtime_error("Not a single successful test execution!");
        }

        pair<unsigned, nanosecond_type> greatestSuccess = findGreatestSuccess(perfTest, lowCount, highCount, lowMaxTime);
        cout << "The greatest success is " << greatestSuccess.first << " simultaneous connections, "
             << "the maximum response time is " << greatestSuccess.second / 1000000 << " ms." << endl;

    }
    catch (const std::exception &e)
    {
        cerr << e.what() << endl;
        return 1;
    }
    return 0;

}
