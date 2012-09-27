#include <algorithm>

#include "util/debug.hpp"

#include "connection_pool.hpp"

namespace paxos { namespace detail {

connection_pool::connection_pool (
   boost::asio::io_service & io_service)
   : io_service_ (io_service)
{
}

tcp_connection &
connection_pool::create ()
{
   //! First dereference the iterator, then the shared_ptr
   return **(connections_.insert (
               connections_.end (),
               tcp_connection::create (io_service_)));
}

void
connection_pool::kill (
   tcp_connection const &       connection)
{
   PAXOS_DEBUG ("killing connection");

   tcp_connection const * p_connection = &connection;
   
   connections_.erase (std::remove_if (connections_.begin (),
                                       connections_.end (),
                                       [p_connection] (tcp_connection::pointer c)
                                       {
                                          return p_connection == c.get ();
                                       }),
                       connections_.end ());

}

}; };
