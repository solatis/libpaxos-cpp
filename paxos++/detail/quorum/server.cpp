#include "../util/debug.hpp"

#include "server.hpp"

namespace paxos { namespace detail { namespace quorum {


/*! static */ std::string 
server::to_string (
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

   PAXOS_FATAL ("Unrecognized server state: " << state);
   PAXOS_UNREACHABLE ();
}


server::server (
   boost::asio::io_service &                    io_service,
   boost::asio::ip::tcp::endpoint const &       endpoint)
   : endpoint_ (endpoint),
     state_ (state_unknown),
     pool_ (io_service,
            endpoint)
{
}

boost::asio::ip::tcp::endpoint const &
server::endpoint () const
{
   return endpoint_;
}


enum server::state
server::state () const
{
   return state_;
}

void
server::set_state (
   enum state        state)
{
   state_ = state;
}

boost::uuids::uuid const &
server::id () const
{
   return id_;
}

void
server::set_id (
   boost::uuids::uuid const &   id)
{
   id_ = id;
}



detail::connection::pool &
server::connection_pool ()
{
   return pool_;
}


detail::connection::pool const &
server::connection_pool () const
{
   return pool_;
}


}; }; };
