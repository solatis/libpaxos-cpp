LDADD = -lboost_system -lboost_serialization -lboost_thread ../paxos++/libpaxos.la

if HAVE_SQLITE
LDADD += -lsqlite3
endif

if HAVE_DEBUG
LDADD += -llog4cxx
endif

check_PROGRAMS = \
	basic1 \
	basic2 \
	basic3 \
	basic4 \
	basic5 \
	connection_close1 \
	connection_close2 \
	durability1 \
	durability2 \
	durability3

basic1_SOURCES      	  = basic1.cpp
basic2_SOURCES      	  = basic2.cpp
basic3_SOURCES      	  = basic3.cpp
basic4_SOURCES      	  = basic4.cpp
basic5_SOURCES      	  = basic5.cpp
connection_close1_SOURCES = connection_close1.cpp
connection_close2_SOURCES = connection_close2.cpp
durability1_SOURCES       = durability1.cpp
durability2_SOURCES       = durability2.cpp
durability3_SOURCES       = durability3.cpp

TESTS= \
	basic1 \
	basic2 \
	basic3 \
	basic4 \
	basic5 \
	connection_close1 \
	connection_close2 \
	durability1 \
	durability2 \
	durability3

