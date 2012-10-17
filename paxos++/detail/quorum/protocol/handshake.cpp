#include "../../util/debug.hpp"
#include "../../command.hpp"
#include "../../parser.hpp"
#include "../../tcp_connection.hpp"
#include "../../command_dispatcher.hpp"
#include "../quorum.hpp"

#include "handshake.hpp"

namespace paxos { namespace detail { namespace quorum { namespace protocol {

/*! static */ void
handshake::step1 (
   boost::asio::io_service &                    io_service,
   boost::asio::ip::tcp::endpoint const &       endpoint,
   tcp_connection_ptr                           connection,
   detail::quorum::quorum &                     quorum)
{
   command command;
   command.set_type (command::type_handshake_start);
   command.set_host_state (quorum.our_state ());

   if (command.host_state () != server::state_client)
   {
      PAXOS_ASSERT (quorum.we_have_our_endpoint () == true);

      command.set_host_endpoint (quorum.our_endpoint ());
      command.set_host_id (quorum.our_id ());
      command.set_live_servers (quorum.live_server_endpoints ());
   }

   /*!
     Writing this command to the connection will make the remote end enter
     step2 ().
   */   
   connection->write_command (command);

   /*!
     We're expecting a response to the handshake.
   */
   connection->read_command (
      [endpoint,
       connection,
       & quorum] (
          boost::optional <enum paxos::error_code>      error,
          detail::command const &                       command)
      {
         if (error)
         {
            quorum.mark_dead (endpoint);
            return;
         }

         PAXOS_ASSERT (command.type () == command::type_handshake_response);
         PAXOS_ASSERT (command.host_state () != server::state_client);
         
         quorum.set_host_state (command.host_endpoint (), command.host_state ());
         quorum.set_host_id (command.host_endpoint (), command.host_id ());
         quorum.lookup_server (command.host_endpoint ()).set_live_servers (command.live_servers ());

         if (quorum.we_have_a_leader () == true
             && quorum.who_is_our_leader () != quorum.who_should_be_leader ())
         {
            PAXOS_WARN ("quorum is in an inconsistent state, " << quorum.who_is_our_leader () << " != " << quorum.who_should_be_leader ());
            quorum.reset ();
         }

      });
}

/*! static */ void
handshake::step2 (
   tcp_connection_ptr           connection,
   detail::command const &      command,
   detail::quorum::quorum &     quorum)
{
   /*!
     Only servers can receive handshake requests, and servers must *always* have
     an endpoint.
    */
   PAXOS_ASSERT (quorum.we_have_our_endpoint () == true);

   if (command.host_state () != server::state_client)
   {
      quorum.set_host_state (command.host_endpoint (), command.host_state ());
      quorum.set_host_id (command.host_endpoint (), command.host_id ());
      quorum.lookup_server (command.host_endpoint ()).set_live_servers (command.live_servers ());
   }

   /*!
     Now reply with our own information.
    */
   detail::command response;
   response.set_type (command::type_handshake_response);
   response.set_host_state (quorum.our_state ());
   response.set_host_endpoint (quorum.our_endpoint ());
   response.set_host_id (quorum.our_id ());
   response.set_live_servers (quorum.live_server_endpoints ());

   connection->write_command (response);

   if (quorum.we_have_a_leader () == true
       && quorum.who_is_our_leader () != quorum.who_should_be_leader ())
   {
      PAXOS_WARN ("quorum is in an inconsistent state, " << quorum.who_is_our_leader () << " != " << quorum.who_should_be_leader ());
      quorum.reset ();
   }
}


}; }; }; };
