ACLOCAL_AMFLAGS = -I config

LIBTOOL_DEPS = @LIBTOOL_DEPS@
libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status libtool


AM_CXXFLAGS = $(SSH_CACHE_CXXFLAGS)
AM_CPPFLAGS = $(SSH_CACHE_CPPFLAGS)
AM_LDFLAGS = $(SSH_CACHE_LDFLAGS)
LDADD = $(SSH_CACHE_LIBS)

sbin_PROGRAMS = ssh_cache

ssh_cache_SOURCES = src/SSHCache.cpp src/ClientConnection.cpp
