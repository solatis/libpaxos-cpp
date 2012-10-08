#include "util/debug.hpp"

#include "../exception/exception.hpp"

#include "quorum/protocol/handshake.hpp"
#include "quorum/protocol/announce_leadership.hpp"
#include "strategies/basic_paxos/protocol/request.hpp"

#include "command.hpp"
#include "dispatcher.hpp"

namespace paxos { namespace detail {

/*! static */ void
dispatcher::dispatch_command (
   tcp_connection::pointer      connection,
   detail::command const &      command,
   detail::quorum::quorum &     quorum,
   detail::paxos_state &        state)
{

   switch (command.type ())
   {
         case command::type_handshake_start:
            quorum::protocol::handshake::step2 (connection,
                                                command,
                                                quorum);
            break;

         case command::type_leader_claim:
            quorum::protocol::announce_leadership::step2 (connection,
                                                          command,
                                                          quorum);
            break;


         case command::type_request_initiate:
            strategies::basic_paxos::protocol::request::step1 (connection,
                                                               command,
                                                               quorum,
                                                               state);
            break;

         case command::type_request_prepare:
            strategies::basic_paxos::protocol::request::step3 (connection,
                                                               command,
                                                               quorum,
                                                               state);
            break;

         case command::type_request_accept:
            strategies::basic_paxos::protocol::request::step6 (connection,
                                                               command,
                                                               quorum,
                                                               state);
            break;

         default:
            /*!
              This means an unexpected command was received!
             */
            PAXOS_THROW (paxos::exception::protocol_error ());
   };

};


}; };
