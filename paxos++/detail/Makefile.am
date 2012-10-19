include_HEADERS = \
	client/protocol/initiate_request.hpp \
	quorum/quorum.hpp \
	quorum/quorum.inl \
	quorum/server.hpp \
	request_queue/queue.hpp \
	request_queue/queue.inl \
	strategy/request.hpp \
	strategy/request.inl \
	strategy/basic_paxos/factory.hpp \
	strategy/basic_paxos/protocol/strategy.hpp \
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
	tcp_connection.hpp 


noinst_LTLIBRARIES=libpaxos_detail.la
libpaxos_detail_la_SOURCES = \
	client/protocol/initiate_request.cpp \
	quorum/quorum.cpp \
	quorum/server.cpp \
	strategy/basic_paxos/factory.cpp \
	strategy/basic_paxos/protocol/strategy.cpp \
	command.cpp \
	command_dispatcher.cpp \
	io_thread.cpp \
	parser.cpp \
	paxos_context.cpp \
	tcp_connection.cpp
