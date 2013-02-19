SUBDIRS = . tests

ACLOCAL_AMFLAGS = -I config

LIBTOOL_DEPS = @LIBTOOL_DEPS@
libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status libtool


ssh_cache_CXXFLAGS = $(SSH_CACHE_CXXFLAGS)
ssh_cache_CPPFLAGS = $(SSH_CACHE_CPPFLAGS)
ssh_cache_LDFLAGS = $(SSH_CACHE_LDFLAGS)
ssh_cache_LDADD = $(SSH_CACHE_LIBS)

sbin_PROGRAMS = ssh_cache

ssh_cache_SOURCES = src/main.cpp \
	src/Server.hpp src/Server.cpp \
	src/ClientConnection.hpp src/ClientConnection.cpp \
	src/SocketUtils.hpp src/SocketUtils.cpp \
	src/Client.hpp src/Client.cpp \
	src/Options.hpp src/Options.cpp
