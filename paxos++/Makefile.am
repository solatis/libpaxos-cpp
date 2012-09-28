include_HEADERS = \
	paxos.hpp \
	quorum.hpp \
	quorum.inl \
	local_server.hpp \
	detail/remote_server.hpp \
	detail/tcp_connection.hpp \
	detail/tcp_connection.inl \
	detail/protocol/protocol.hpp \
	detail/protocol/handshake.hpp \
	detail/protocol/elect_leader.hpp \
	detail/protocol/announce_leadership.hpp \
	detail/protocol/command.hpp \
	detail/protocol/command.inl \
	detail/util/conversion.hpp \
	detail/util/conversion.inl \
	detail/util/debug.hpp

lib_LTLIBRARIES=libpaxos.la
libpaxos_la_SOURCES = \
	paxos.cpp \
	quorum.cpp \
	local_server.cpp \
	detail/remote_server.cpp \
	detail/tcp_connection.cpp \
	detail/protocol/protocol.cpp \
	detail/protocol/handshake.cpp \
	detail/protocol/elect_leader.cpp \
	detail/protocol/announce_leadership.cpp \
	detail/protocol/command.cpp 


