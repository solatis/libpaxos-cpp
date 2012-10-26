include_HEADERS = \
	detail/client/protocol/request.hpp \
	detail/client/protocol/initiate_request.hpp \
	detail/quorum/client_view.hpp \
	detail/quorum/server_view.hpp \
	detail/quorum/view.hpp \
	detail/quorum/server.hpp \
	detail/request_queue/queue.hpp \
	detail/request_queue/queue.inl \
	detail/strategy/factory.hpp \
	detail/strategy/factory.inl \
	detail/strategy/request.hpp \
	detail/strategy/request.inl \
	detail/strategy/strategy.hpp \
	detail/strategy/strategy.inl \
	detail/strategy/basic_paxos/factory.hpp \
	detail/strategy/basic_paxos/protocol/strategy.hpp \
	detail/util/conversion.hpp \
	detail/util/conversion.inl \
	detail/util/debug.hpp \
	detail/command.hpp \
	detail/command.inl \
	detail/command_dispatcher.hpp \
	detail/error.hpp \
	detail/io_thread.hpp \
	detail/parser.hpp \
	detail/paxos_context.hpp \
	detail/paxos_context.inl \
	detail/tcp_connection.hpp \
	detail/tcp_connection_fwd.hpp \
	durable/heap.hpp \
	durable/storage.hpp \
	exception/exception.hpp \
	client.hpp \
	configuration.hpp \
	server.hpp


lib_LTLIBRARIES=libpaxos.la
libpaxos_la_SOURCES = \
	detail/client/protocol/initiate_request.cpp \
	detail/quorum/client_view.cpp \
	detail/quorum/server_view.cpp \
	detail/quorum/view.cpp \
	detail/quorum/server.cpp \
	detail/strategy/basic_paxos/factory.cpp \
	detail/strategy/basic_paxos/protocol/strategy.cpp \
	detail/command.cpp \
	detail/command_dispatcher.cpp \
	detail/error.cpp \
	detail/io_thread.cpp \
	detail/parser.cpp \
	detail/paxos_context.cpp \
	detail/tcp_connection.cpp \
	durable/heap.cpp \
	durable/storage.cpp \
	client.cpp \
	configuration.cpp \
	server.cpp

if HAVE_SQLITE
include_HEADERS += durable/sqlite.hpp
libpaxos_la_SOURCES += durable/sqlite.cpp
endif

