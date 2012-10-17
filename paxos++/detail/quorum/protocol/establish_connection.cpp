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
   tcp_connection_ptr control_connection   = tcp_connection::create (io_service);
   tcp_connection_ptr broadcast_connection = tcp_connection::create (io_service);

   PAXOS_ASSERT (control_connection->socket ().is_open () == false);
   PAXOS_ASSERT (broadcast_connection->socket ().is_open () == false);

   broadcast_connection->socket ().async_connect (endpoint,
                                                  [& endpoint,
                                                   & quorum,
                                                   broadcast_connection]
                                                  (boost::system::error_code const & error)
                                                  {
                                                     if (error)
                                                     {
                                                        quorum.mark_dead (endpoint);
                                                     }
                                                     else
                                                     {
                                                        quorum.broadcast_connection_established (endpoint, 
                                                                                                 broadcast_connection);
                                                     }
                                                  });

   control_connection->socket ().async_connect (endpoint,
                                                [& endpoint,
                                                 & quorum,
                                                 control_connection]
                                                (boost::system::error_code const & error)
                                                {
                                                   if (error)
                                                   {
                                                      quorum.mark_dead (endpoint);
                                                   }
                                                   else
                                                   {
                                                      quorum.control_connection_established (endpoint, 
                                                                                             control_connection);
                                                   }
                                                });
}

}; }; }; };
