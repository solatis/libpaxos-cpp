include_HEADERS = \
	client/protocol/initiate_request.hpp \
	connection/pool.hpp \
	connection/scoped_connection.hpp \
	connection/tcp_connection.hpp \
	quorum/quorum.hpp \
	quorum/quorum.inl \
	quorum/server.hpp \
	quorum/protocol/announce_leadership.hpp \
	quorum/protocol/handshake.hpp \
	strategies/basic_paxos/protocol/request.hpp \
	util/conversion.hpp \
	util/conversion.inl \
	util/debug.hpp \
	command.hpp \
	command.inl \
	dispatcher.hpp \
	io_thread.hpp \
	parser.hpp


noinst_LTLIBRARIES=libpaxos_detail.la
libpaxos_detail_la_SOURCES = \
	client/protocol/initiate_request.cpp \
	connection/pool.cpp \
	connection/scoped_connection.cpp \
	connection/tcp_connection.cpp \
	quorum/quorum.cpp \
	quorum/server.cpp \
	quorum/protocol/announce_leadership.cpp \
	quorum/protocol/handshake.cpp \
	strategies/basic_paxos/protocol/request.cpp \
	command.cpp \
	dispatcher.cpp \
	io_thread.cpp \
	parser.cpp 
