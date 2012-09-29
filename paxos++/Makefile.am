SUBDIRS = detail

include_HEADERS = \
	exception/exception.hpp \
	client.hpp \
	configuration.hpp \
	quorum.hpp \
	server.hpp 

lib_LTLIBRARIES=libpaxos.la
libpaxos_la_SOURCES = \
	client.cpp \
	configuration.cpp \
	quorum.cpp \
	server.cpp 

libpaxos_la_LIBADD = \
	detail/libpaxos_detail.la \
	detail/protocol/libpaxos_detail_protocol.la
