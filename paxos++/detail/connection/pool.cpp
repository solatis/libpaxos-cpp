#include "../util/debug.hpp"
#include <boost/asio/placeholders.hpp>

#include "pool.hpp"

namespace paxos { namespace detail { namespace connection {

pool::pool (
   boost::asio::io_service &                    io_service,
   boost::asio::ip::tcp::endpoint const &       endpoint,
   uint16_t                                     min_spare,
   uint16_t                                     max_spare)
   : io_service_ (io_service),
     endpoint_ (endpoint),
     min_spare_ (min_spare),
     max_spare_ (max_spare)
{
}


void
pool::check_spare_connections ()
{
   while (connections_.size () > max_spare_)
   {
      connections_.pop ();
   }

   int16_t connections_to_allocate = min_spare_ - connections_.size ();

   PAXOS_DEBUG ("allocating " << connections_to_allocate << " new connections!");

   for (int16_t i = 0; i < connections_to_allocate; ++i)
   {
      tcp_connection::pointer connection = tcp_connection::create (io_service_);
      connection->socket ().async_connect (endpoint_,
                                           boost::bind (&pool::connection_established,
                                                        this,
                                                        boost::asio::placeholders::error,
                                                        connection));
   }
}

void
pool::connection_established (
   boost::system::error_code const &         error,
   tcp_connection::pointer                   connection)
{
   if (error)
   {
      PAXOS_WARN ("unable to establish connection with " << endpoint_ << ": " << error.message ());
      return;
   }

   connections_.push (connection);
}



bool
pool::has_spare_connections () const
{
   return connections_.empty () == false;
}


scoped_connection::pointer
pool::connection ()
{
   return scoped_connection::create (*this);
}

tcp_connection::pointer
pool::acquire ()
{
   tcp_connection::pointer connection = connections_.front ();
   connections_.pop ();

   return connection;
}

void
pool::release (
   tcp_connection::pointer      connection)
{
   PAXOS_DEBUG ("releasing connection " << connection.get ());
   connections_.push (connection);
}



}; }; };
