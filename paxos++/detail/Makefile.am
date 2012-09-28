SUBDIRS = protocol

include_HEADERS = \
	quorum.hpp \
	quorum.inl \
	remote_server.hpp \
	tcp_connection.hpp \
	tcp_connection.inl \
	util/conversion.hpp \
	util/conversion.inl \
	util/debug.hpp


noinst_LTLIBRARIES=libpaxos_detail.la
libpaxos_detail_la_SOURCES = \
	quorum.cpp \
	remote_server.cpp \
	tcp_connection.cpp 
