SUBDIRS = detail

include_HEADERS = \
	configuration.hpp \
	quorum.hpp \
	server.hpp

lib_LTLIBRARIES=libpaxos.la
libpaxos_la_SOURCES = \
	configuration.cpp \
	quorum.cpp \
	server.cpp 

libpaxos_la_LIBADD = \
	detail/libpaxos_detail.la \
	detail/protocol/libpaxos_detail_protocol.la
