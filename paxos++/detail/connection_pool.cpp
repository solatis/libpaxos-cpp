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

}; };
