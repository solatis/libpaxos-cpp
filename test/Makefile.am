LDADD = -lboost_system -lboost_serialization -lboost_thread -lboost_filesystem ../paxos++/libpaxos.la $(LINK_LOG4CXX)

if HAVE_SQLITE
LDADD += -lsqlite3
endif

check_PROGRAMS = \
	basic1 \
	basic2 \
	basic3 \
	basic4 \
	basic5 \
	connection_close1 \
	connection_close2 \
	durability1

basic1_SOURCES      	  = basic1.cpp
basic2_SOURCES      	  = basic2.cpp
basic3_SOURCES      	  = basic3.cpp
basic4_SOURCES      	  = basic4.cpp
basic5_SOURCES      	  = basic5.cpp
connection_close1_SOURCES = connection_close1.cpp
connection_close2_SOURCES = connection_close2.cpp
durability1_SOURCES       = durability1.cpp

TESTS= \
	basic1 \
	basic2 \
	basic3 \
	basic4 \
	basic5 \
	connection_close1 \
	connection_close2 \
	durability1

