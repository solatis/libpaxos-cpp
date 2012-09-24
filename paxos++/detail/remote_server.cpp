#include "remote_server.hpp"

namespace paxos { namespace detail {

remote_server::remote_server ()
   : state_ (state_dead)
{
}

void
remote_server::set_state (
   enum state   state)
{
   state_ = state;
}

enum remote_server::state
remote_server::state () const
{
   return state_;
}


void
remote_server::set_connection (
   tcp_connection::pointer      connection)
{
   connection_ = connection;
}

bool
remote_server::has_connection ()
{
   return connection_.is_initialized () == true;
}

tcp_connection &
remote_server::connection ()
{
   //! Dereference boost optional and pointer
   return **connection_;
}

tcp_connection const &
remote_server::connection () const
{
   //! Dereference boost optional and pointer
   return **connection_;
}


}; };
