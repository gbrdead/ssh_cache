#include "PerfTests.hpp"
#include "PerfTestsOptions.hpp"
using namespace org::voidland::ssh_cache::test::performance;

#include <iostream>
using namespace std;


int main(int argc, char *argv[])
{
    Options options(argc, argv);
    if (options.isHelp())
    {
        cout << options.getDescription();
        return 0;
    }

    PerformanceTest perfTest(options);
    if (!perfTest.execute(100))
    {
        cerr << "Error" << endl;
    }
    else
    {
        cout << perfTest.getMaxTime() / 1000000 << " ms" << endl;
    }
}
