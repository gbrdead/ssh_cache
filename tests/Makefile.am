noinst_PROGRAMS =


if SSH_CACHE_TESTS

noinst_PROGRAMS += ssh_cache_tests

ssh_cache_tests_CXXFLAGS = $(SSH_CACHE_TESTS_CXXFLAGS)
ssh_cache_tests_CPPFLAGS = $(SSH_CACHE_TESTS_CPPFLAGS)
ssh_cache_tests_LDFLAGS = $(SSH_CACHE_TESTS_LDFLAGS)
ssh_cache_tests_LDADD = $(SSH_CACHE_TESTS_LIBS)

ssh_cache_tests_SOURCES = src/main.cpp \
	src/OptionsTests.cpp src/TCPTests.cpp \
	src/TestUtils.hpp src/TestUtils.cpp \
	src/TestServer.hpp src/TestServer.cpp \
	src/RandomResponseServer.hpp src/RandomResponseServer.cpp \
	../src/SSHCache.hpp ../src/SSHCache.cpp \
	../src/Server.hpp ../src/Server.cpp \
	../src/ClientConnection.hpp ../src/ClientConnection.cpp \
	../src/SocketUtils.hpp ../src/SocketUtils.cpp \
	../src/Client.hpp ../src/Client.cpp \
	../src/Options.hpp ../src/Options.cpp

TESTS = ssh_cache_tests

endif


if SSH_CACHE_PERF_TESTS

noinst_PROGRAMS += ssh_cache_perf_tests

ssh_cache_perf_tests_CXXFLAGS = $(SSH_CACHE_PERF_TESTS_CXXFLAGS)
ssh_cache_perf_tests_CPPFLAGS = $(SSH_CACHE_PERF_TESTS_CPPFLAGS)
ssh_cache_perf_tests_LDFLAGS = $(SSH_CACHE_PERF_TESTS_LDFLAGS)
ssh_cache_perf_tests_LDADD = $(SSH_CACHE_PERF_TESTS_LIBS)

ssh_cache_perf_tests_SOURCES = src/perf/main.cpp \
	src/perf/PerfTests.hpp src/perf/PerfTests.cpp \
	src/perf/PerfTestsOptions.hpp src/perf/PerfTestsOptions.cpp \
	src/TestUtils.hpp src/TestUtils.cpp \
	../src/SocketUtils.hpp ../src/SocketUtils.cpp 

endif

noinst_PROGRAMS += ssh_cache_perf

ssh_cache_perf_CXXFLAGS = $(SSH_CACHE_PERF_CXXFLAGS)
ssh_cache_perf_CPPFLAGS = $(SSH_CACHE_PERF_CPPFLAGS)
ssh_cache_perf_LDFLAGS = $(SSH_CACHE_PERF_LDFLAGS)
ssh_cache_perf_LDADD = $(SSH_CACHE_PERF_LIBS)

ssh_cache_perf_SOURCES = src/perf/mainServer.cpp \
	src/perf/EchoServer.hpp src/perf/EchoServer.cpp \
	src/TestUtils.hpp src/TestUtils.cpp \
	src/TestServer.hpp src/TestServer.cpp \
	../src/SSHCache.hpp ../src/SSHCache.cpp \
	../src/Server.hpp ../src/Server.cpp \
	../src/ClientConnection.hpp ../src/ClientConnection.cpp \
	../src/SocketUtils.hpp ../src/SocketUtils.cpp \
	../src/Client.hpp ../src/Client.cpp \
	../src/Options.hpp ../src/Options.cpp
