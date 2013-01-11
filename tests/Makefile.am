AUTOMAKE_OPTIONS = subdir-objects

if BOOST_UNIT_TEST_FRAMEWORK

AM_CXXFLAGS = $(SSH_CACHE_CXXFLAGS) $(BOOST_UNIT_TEST_FRAMEWORK_CXXFLAGS)
AM_CPPFLAGS = $(SSH_CACHE_CPPFLAGS) -I $(top_srcdir)/src
AM_LDFLAGS = $(SSH_CACHE_LDFLAGS)
LDADD = $(SSH_CACHE_LIBS) $(BOOST_UNIT_TEST_FRAMEWORK_LIBS)

noinst_PROGRAMS = ssh_cache_tests

ssh_cache_tests_SOURCES = src/main.cpp \
	src/OptionsTests.cpp src/TCPTests.cpp \
	src/TestUtils.hpp src/TestUtils.cpp \
	src/RandomResponseServer.hpp src/RandomResponseServer.cpp \
	../src/SSHCache.hpp ../src/SSHCache.cpp \
	../src/Server.hpp ../src/Server.cpp \
	../src/ClientConnection.hpp ../src/ClientConnection.cpp \
	../src/SocketUtils.hpp ../src/SocketUtils.cpp \
	../src/Client.hpp ../src/Client.cpp \
	../src/Options.hpp ../src/Options.cpp

TESTS = ssh_cache_tests

endif
