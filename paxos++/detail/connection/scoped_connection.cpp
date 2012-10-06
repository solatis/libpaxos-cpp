#include "pool.hpp"
#include "scoped_connection.hpp"

namespace paxos { namespace detail { namespace connection {

scoped_connection::scoped_connection (
   pool &    pool)
   : pool_ (pool),
     connection_ (pool_.acquire ())
{
}

/*! static */ scoped_connection::pointer
scoped_connection::create (
   pool &       pool)
{
   return pointer (
      new scoped_connection (pool));
}


void
scoped_connection::done ()
{
   pool_.release (connection_);
}


tcp_connection::pointer
scoped_connection::connection ()
{
   return connection_;
}


}; }; };
