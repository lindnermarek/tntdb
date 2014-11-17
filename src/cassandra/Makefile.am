AM_CPPFLAGS = -I$(top_srcdir)/include -I$(top_builddir)/include

sources = connection.cpp

if MAKE_CASSANDRA

driver_LTLIBRARIES = tntdb@abi_current@-cassandra.la

tntdb@abi_current@_cassandra_la_SOURCES = $(sources)
tntdb@abi_current@_cassandra_la_LDFLAGS = -module -version-info @sonumber@ -lcassandra @SHARED_LIB_FLAG@
tntdb@abi_current@_cassandra_la_LIBADD = $(top_builddir)/src/libtntdb.la

endif
