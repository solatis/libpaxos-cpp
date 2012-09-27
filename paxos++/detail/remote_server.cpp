#include "util/debug.hpp"
#include "remote_server.hpp"

namespace paxos { namespace detail {

/*! static */ std::string 
remote_server::to_string (
   enum state state)
{
   switch (state)
   {
         case state_unknown:
            return "state_unknown";

         case state_dead:
            return "state_dead";

         case state_non_participant:
            return "state_non_participant";

         case state_leader:
            return "state_leader";

         case state_follower:
            return "state_follower";

         case state_client:
            return "state_client";      
   };

   PAXOS_UNREACHABLE ();
}


remote_server::remote_server ()
   : state_ (state_unknown)
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
