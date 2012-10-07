#include "../util/debug.hpp"

#include "pool.hpp"
#include "scoped_connection.hpp"

namespace paxos { namespace detail { namespace connection {

scoped_connection::scoped_connection (
   pool &    pool)
   : pool_ (pool),
     connection_ (pool_.acquire ()),
     done_ (false)
{
}

scoped_connection::~scoped_connection ()
{
   if (done_ == true)
   {
      pool_.release (connection_);
   }
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
   PAXOS_ASSERT (done_ == false);
   done_ = true;
}


tcp_connection::pointer
scoped_connection::connection ()
{
   return connection_;
}


}; }; };
