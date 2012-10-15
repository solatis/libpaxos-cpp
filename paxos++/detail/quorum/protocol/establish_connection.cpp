#include <functional>
#include <boost/asio/placeholders.hpp>

#include "../../util/debug.hpp"
#include "../../tcp_connection.hpp"
#include "../quorum.hpp"

#include "establish_connection.hpp"

namespace paxos { namespace detail { namespace quorum { namespace protocol {

/*! static */ void
establish_connection::step1 (
   boost::asio::io_service &                    io_service,
   boost::asio::ip::tcp::endpoint const &       endpoint,
   detail::quorum::quorum &                     quorum)
{
   tcp_connection_ptr connection = tcp_connection::create (io_service);

   PAXOS_ASSERT (connection->socket ().is_open () == false);

   connection->socket ().async_connect (endpoint,
                                        std::bind (&establish_connection::step2,
                                                   std::placeholders::_1,
                                                   std::ref (endpoint),
                                                   connection,
                                                   std::ref (quorum)));
}

/*! static */ void
establish_connection::step2 (
   boost::system::error_code const &            error,
   boost::asio::ip::tcp::endpoint const &       endpoint,
   tcp_connection_ptr                           connection,
   detail::quorum::quorum &                     quorum)
{
   if (error)
   {
      PAXOS_WARN ("An error occured while establishing a connection with node " << endpoint << ": " << error.message ());

      quorum.mark_dead (endpoint);
      return;
   }

   PAXOS_DEBUG ("established connection = " << connection.get ());
   quorum.connection_established (endpoint, connection);

   /*!
     After a new connection has been established, ensure our command_dispatcher
     goes in a never-ending loop to parse commands.

     Note that we do *not* provide a so-called "stateless command" callback 
     handler here, because we *only* expect this connection to send commands
     in response to commands we initiate (handshakes, leadership announcements,
     etc). So this is a response-only connection, and will assert on any commands
     that are initiated by the connection itself.
    */
   connection->command_dispatcher ().read_loop ();
}


}; }; }; };
