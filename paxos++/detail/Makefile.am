include_HEADERS = \
	client/protocol/initiate_request.hpp \
	quorum/quorum.hpp \
	quorum/quorum.inl \
	quorum/server.hpp \
	quorum/protocol/announce_leadership.hpp \
	quorum/protocol/establish_connection.hpp \
	quorum/protocol/handshake.hpp \
	strategies/basic_paxos/protocol/request.hpp \
	util/conversion.hpp \
	util/conversion.inl \
	util/debug.hpp \
	command.hpp \
	command.inl \
	command_dispatcher.hpp \
	io_thread.hpp \
	parser.hpp \
	paxos_context.hpp \
	paxos_context.inl \
	paxos_request.hpp \
	paxos_request.inl \
	paxos_request_queue.hpp \
	paxos_request_queue.inl \
	tcp_connection.hpp 


noinst_LTLIBRARIES=libpaxos_detail.la
libpaxos_detail_la_SOURCES = \
	client/protocol/initiate_request.cpp \
	quorum/quorum.cpp \
	quorum/server.cpp \
	quorum/protocol/announce_leadership.cpp \
	quorum/protocol/establish_connection.cpp \
	quorum/protocol/handshake.cpp \
	strategies/basic_paxos/protocol/request.cpp \
	command.cpp \
	command_dispatcher.cpp \
	io_thread.cpp \
	parser.cpp \
	paxos_request_queue.cpp \
	tcp_connection.cpp
