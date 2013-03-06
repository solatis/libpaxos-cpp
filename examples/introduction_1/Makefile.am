LDADD = ../../paxos++/libpaxos.la  -lboost_system -lboost_serialization -lboost_thread $(LINK_LOG4CXX)

if HAVE_SQLITE
LDADD += -lsqlite3
endif

if HAVE_DEBUG
LDADD += -llog4cxx
endif

bin_PROGRAMS = client server

server_SOURCES = server.cpp
client_SOURCES = client.cpp
