SUBDIRS = . tests

AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = -I config

LIBTOOL_DEPS = @LIBTOOL_DEPS@
libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status libtool


noinst_LTLIBRARIES = libssh_cache.la
sbin_PROGRAMS = ssh_cache


libssh_cache_la_CXXFLAGS = $(SSH_CACHE_CXXFLAGS)
libssh_cache_la_CPPFLAGS = $(SSH_CACHE_CPPFLAGS)
libssh_cache_la_LDFLAGS = $(SSH_CACHE_LDFLAGS)
libssh_cache_la_LIBADD = $(SSH_CACHE_LIBS)

libssh_cache_la_SOURCES = \
	src/Server.hpp src/Server.cpp \
	src/ClientConnection.hpp src/ClientConnection.cpp \
	src/SocketUtils.hpp src/SocketUtils.cpp \
	src/Client.hpp src/Client.cpp \
	src/Options.hpp src/Options.cpp


ssh_cache_CXXFLAGS = $(SSH_CACHE_CXXFLAGS)
ssh_cache_CPPFLAGS = $(SSH_CACHE_CPPFLAGS)
ssh_cache_LDFLAGS = $(SSH_CACHE_LDFLAGS)
ssh_cache_LDADD = $(SSH_CACHE_LIBS) libssh_cache.la

ssh_cache_SOURCES = src/main.cpp
