#include "Options.hpp"

#include <boost/test/unit_test.hpp>


namespace org
{
namespace voidland
{
namespace ssh_cache
{
namespace test
{


BOOST_AUTO_TEST_CASE(OptionsTestPositionalOptions)
{
    const char *argv[] =
    {
        "ssh_cache_tests",
        "alabala"
    };
    int argc = sizeof(argv) / sizeof(argv[0]);

    BOOST_REQUIRE_THROW(Options(argc, argv), too_many_positional_options_error);
}

BOOST_AUTO_TEST_CASE(OptionsTestDefaultValues)
{
    const char *argv[] =
    {
        "ssh_cache_tests",
    };
    int argc = sizeof(argv) / sizeof(argv[0]);

    Options options(argc, argv);

    BOOST_CHECK_EQUAL(options.getPort(), 8022);
    BOOST_CHECK_EQUAL(options.getRealBackendHost(), "localhost");
    BOOST_CHECK_EQUAL(options.getRealBackendPort(), "8023");
    BOOST_CHECK_EQUAL(options.getFakeBackendHost(), "localhost");
    BOOST_CHECK_EQUAL(options.getFakeBackendPort(), "8024");
    BOOST_CHECK_EQUAL(options.getInitialMitmAttacks(), 1);
    BOOST_CHECK_EQUAL(options.getClientExpirationInS(), 3600);
}

BOOST_AUTO_TEST_CASE(OptionsTestHelp)
{
    const char *argv[] =
    {
        "ssh_cache_tests",
        "--help"
    };
    int argc = sizeof(argv) / sizeof(argv[0]);

    Options options(argc, argv);

    BOOST_REQUIRE(options.isHelp());
}

BOOST_AUTO_TEST_CASE(OptionsTestPort)
{
    const char *argv[] =
    {
        "ssh_cache_tests",
        "--port",
        "8020"
    };
    int argc = sizeof(argv) / sizeof(argv[0]);

    Options options(argc, argv);

    BOOST_REQUIRE_EQUAL(options.getPort(), 8020);
}

BOOST_AUTO_TEST_CASE(OptionsTestPortInvalid1)
{
    const char *argv[] =
    {
        "ssh_cache_tests",
        "--port",
        "123456"
    };
    int argc = sizeof(argv) / sizeof(argv[0]);

    BOOST_REQUIRE_THROW(Options(argc, argv), invalid_option_value);
}

BOOST_AUTO_TEST_CASE(OptionsTestPortInvalid2)
{
    const char *argv[] =
    {
        "ssh_cache_tests",
        "--port",
        "alabala"
    };
    int argc = sizeof(argv) / sizeof(argv[0]);

    BOOST_REQUIRE_THROW(Options(argc, argv), invalid_option_value);
}

BOOST_AUTO_TEST_CASE(OptionsTestRealBackendHost)
{
    const char *argv[] =
    {
        "ssh_cache_tests",
        "--real-backend-host",
        "rhst"
    };
    int argc = sizeof(argv) / sizeof(argv[0]);

    Options options(argc, argv);

    BOOST_REQUIRE_EQUAL(options.getRealBackendHost(), "rhst");
}

BOOST_AUTO_TEST_CASE(OptionsTestRealBackendPort)
{
    const char *argv[] =
    {
        "ssh_cache_tests",
        "--real-backend-host",
        "rprt"
    };
    int argc = sizeof(argv) / sizeof(argv[0]);

    Options options(argc, argv);

    BOOST_REQUIRE_EQUAL(options.getRealBackendHost(), "rprt");
}

BOOST_AUTO_TEST_CASE(OptionsTestFakeBackendHost)
{
    const char *argv[] =
    {
        "ssh_cache_tests",
        "--fake-backend-host",
        "fhst"
    };
    int argc = sizeof(argv) / sizeof(argv[0]);

    Options options(argc, argv);

    BOOST_REQUIRE_EQUAL(options.getFakeBackendHost(), "fhst");
}

BOOST_AUTO_TEST_CASE(OptionsTestFakeBackendPort)
{
    const char *argv[] =
    {
        "ssh_cache_tests",
        "--fake-backend-host",
        "fprt"
    };
    int argc = sizeof(argv) / sizeof(argv[0]);

    Options options(argc, argv);

    BOOST_REQUIRE_EQUAL(options.getFakeBackendHost(), "fprt");
}

BOOST_AUTO_TEST_CASE(OptionsTestInitialMitmAttacks)
{
    const char *argv[] =
    {
        "ssh_cache_tests",
        "--initial-mitm-attacks",
        "5"
    };
    int argc = sizeof(argv) / sizeof(argv[0]);

    Options options(argc, argv);

    BOOST_REQUIRE_EQUAL(options.getInitialMitmAttacks(), 5);
}

BOOST_AUTO_TEST_CASE(OptionsTestInitialMitmAttacksInvalid)
{
    const char *argv[] =
    {
        "ssh_cache_tests",
        "--initial-mitm-attacks",
        "alabala"
    };
    int argc = sizeof(argv) / sizeof(argv[0]);

    BOOST_REQUIRE_THROW(Options(argc, argv), invalid_option_value);
}

BOOST_AUTO_TEST_CASE(OptionsTestClientExpiration)
{
    const char *argv[] =
    {
        "ssh_cache_tests",
        "--client-expiration",
        "1"
    };
    int argc = sizeof(argv) / sizeof(argv[0]);

    Options options(argc, argv);

    BOOST_REQUIRE_EQUAL(options.getClientExpirationInS(), 1);
}

BOOST_AUTO_TEST_CASE(OptionsTestClientExpirationInvalid)
{
    const char *argv[] =
    {
        "ssh_cache_tests",
        "--client-expiration",
        "alabala"
    };
    int argc = sizeof(argv) / sizeof(argv[0]);

    BOOST_REQUIRE_THROW(Options(argc, argv), invalid_option_value);
}

}
}
}
}
