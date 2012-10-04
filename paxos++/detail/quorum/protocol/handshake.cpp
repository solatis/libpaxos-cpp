#include <boost/uuid/uuid_io.hpp>

#include "../../util/debug.hpp"
#include "../../command.hpp"
#include "../../parser.hpp"
#include "../quorum.hpp"

#include "handshake.hpp"

namespace paxos { namespace detail { namespace quorum { namespace protocol {

/*! static */ void
handshake::step1 (
   boost::asio::io_service &                    io_service,
   tcp_connection::pointer                      connection,
   detail::quorum::quorum &                     quorum)
{
   PAXOS_ASSERT (connection->socket ().is_open () == true);

   
   command command;
   command.set_type (command::type_handshake_start);
   command.set_host_state (quorum.our_state ());

   if (command.host_state () != server::state_client)
   {
      PAXOS_ASSERT (quorum.we_have_our_endpoint () == true);

      command.set_host_endpoint (quorum.our_endpoint ());
      command.set_host_id (quorum.our_id ());
   }

   /*!
     Writing this command to the connection will make the remote end enter
     step2 ().
    */   
   parser::write_command (connection,
                          command);

   /*!
     We're expecting a response to the handshake.
    */
   parser::read_command (connection,
                         [& quorum] (detail::command const &    command)
                         {
                            PAXOS_ASSERT (command.type () == command::type_handshake_response);
                            PAXOS_ASSERT (command.host_state () != server::state_client);
                            
                            quorum.set_host_state (command.host_endpoint (), command.host_state ());
                            quorum.set_host_id (command.host_endpoint (), command.host_id ());
                         });
}

/*! static */ void
handshake::step2 (
   tcp_connection::pointer      connection,
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
   }

   /*!
     Now reply with our own information.
    */
   detail::command response;
   response.set_type (command::type_handshake_response);
   response.set_host_state (quorum.our_state ());
   response.set_host_endpoint (quorum.our_endpoint ());
   response.set_host_id (quorum.our_id ());

   parser::write_command (connection,
                          response);
}


}; }; }; };
