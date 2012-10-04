#include <boost/asio/placeholders.hpp>

#include "../../util/debug.hpp"
#include "../quorum.hpp"

#include "establish_connection.hpp"

namespace paxos { namespace detail { namespace quorum { namespace protocol {

/*! static */ void
establish_connection::step1 (
   boost::asio::io_service &                    io_service,
   boost::asio::ip::tcp::endpoint const &       endpoint,
   detail::quorum::quorum &                     quorum)
{
   tcp_connection::pointer connection = tcp_connection::create (io_service);

   PAXOS_ASSERT (connection->socket ().is_open () == false);

   connection->socket ().async_connect (endpoint,
                                        boost::bind (&establish_connection::step2,
                                                     boost::asio::placeholders::error,
                                                     boost::ref (endpoint),
                                                     connection,
                                                     boost::ref (quorum)));
}

/*! static */ void
establish_connection::step2 (
   boost::system::error_code const &            error,
   boost::asio::ip::tcp::endpoint const &       endpoint,
   tcp_connection::pointer                      connection,
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
}


}; }; }; };
