#include "PerfTests.hpp"
#include "PerfTestsOptions.hpp"
using namespace org::voidland::ssh_cache::test::performance;


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

        PerformanceTestsExecutor perfTestsExecutor(options);
        perfTestsExecutor.execute();
    }
    catch (const std::exception &e)
    {
        cerr << e.what() << endl;
        return 1;
    }
    return 0;
}
