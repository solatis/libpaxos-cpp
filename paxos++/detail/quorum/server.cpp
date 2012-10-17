#include "../util/debug.hpp"

#include "../tcp_connection.hpp"
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
     state_ (state_unknown)
{
}

server::~server ()
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

void
server::reset_id ()
{
   id_ = boost::uuids::uuid ();
}

bool
server::has_connection () const
{
   return 
      broadcast_connection_.is_initialized () == true &&
      control_connection_.is_initialized () == true;
}

void
server::set_broadcast_connection (
   detail::tcp_connection_ptr   connection)
{
   broadcast_connection_ = connection;
}

void
server::set_control_connection (
   detail::tcp_connection_ptr   connection)
{
   control_connection_ = connection;
}


void
server::reset_connection ()
{
   broadcast_connection_ = boost::none;
   control_connection_   = boost::none;
}

detail::tcp_connection_ptr
server::broadcast_connection ()
{
   return *broadcast_connection_;
}

detail::tcp_connection_ptr
server::control_connection ()
{
   return *control_connection_;
}


std::vector <boost::asio::ip::tcp::endpoint> const &
server::live_servers () const
{
   return live_servers_;
}


void
server::set_live_servers (
   std::vector <boost::asio::ip::tcp::endpoint> const & servers)
{
   live_servers_ = servers;
}


}; }; };
