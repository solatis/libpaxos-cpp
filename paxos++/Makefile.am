include_HEADERS = \
	detail/client/protocol/initiate_request.hpp \
	detail/quorum/quorum.hpp \
	detail/quorum/quorum.inl \
	detail/quorum/server.hpp \
	detail/request_queue/queue.hpp \
	detail/request_queue/queue.inl \
	detail/strategy/request.hpp \
	detail/strategy/request.inl \
	detail/strategy/basic_paxos/factory.hpp \
	detail/strategy/basic_paxos/protocol/strategy.hpp \
	detail/util/conversion.hpp \
	detail/util/conversion.inl \
	detail/util/debug.hpp \
	detail/command.hpp \
	detail/command.inl \
	detail/command_dispatcher.hpp \
	detail/io_thread.hpp \
	detail/parser.hpp \
	detail/paxos_context.hpp \
	detail/paxos_context.inl \
	detail/tcp_connection.hpp \
	exception/exception.hpp \
	client.hpp \
	configuration.hpp \
	error.hpp \
	server.hpp

lib_LTLIBRARIES=libpaxos.la
libpaxos_la_SOURCES = \
	detail/client/protocol/initiate_request.cpp \
	detail/quorum/quorum.cpp \
	detail/quorum/server.cpp \
	detail/strategy/basic_paxos/factory.cpp \
	detail/strategy/basic_paxos/protocol/strategy.cpp \
	detail/command.cpp \
	detail/command_dispatcher.cpp \
	detail/io_thread.cpp \
	detail/parser.cpp \
	detail/paxos_context.cpp \
	detail/tcp_connection.cpp \
	client.cpp \
	configuration.cpp \
	error.cpp \
	server.cpp

