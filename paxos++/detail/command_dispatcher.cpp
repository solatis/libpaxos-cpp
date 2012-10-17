#include "../exception/exception.hpp"
#include "util/debug.hpp"

#include "quorum/protocol/handshake.hpp"
#include "quorum/protocol/announce_leadership.hpp"
#include "strategy/strategy.hpp"

#include "tcp_connection.hpp"
#include "paxos_context.hpp"
#include "command_dispatcher.hpp"

namespace paxos { namespace detail {

/*! static */ void
command_dispatcher::dispatch_command (
   boost::optional <enum paxos::error_code>     error,
   tcp_connection_ptr                           connection,
   detail::command const &                      command,
   detail::quorum::quorum &                     quorum,
   detail::paxos_context &                      state)
{
   if (error)
   {
      PAXOS_WARN ("an error occured while reading stateless commands: " << paxos::to_string (*error));
      connection->close ();
      return;
   }

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
            state.request_queue ().push (
               {
                  connection,
                  command,
                  quorum,
                  state
               });
            break;

         case command::type_request_prepare:
            state.strategy ().prepare (connection,
                                       command,
                                       quorum,
                                       state);
            break;      

         case command::type_request_accept:
            state.strategy ().accept (connection,
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
