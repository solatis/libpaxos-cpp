#include "detail/util/debug.hpp"
#include "quorum.hpp"

namespace paxos { 


void
quorum::we_are (
   boost::asio::ip::address const &     address,
   uint16_t                             port,
   boost::uuids::uuid const &           id,
   enum detail::remote_server::state    state)
{
   self_.endpoint_ = boost::asio::ip::tcp::endpoint (address, port);
   self_.id_       = id;
   self_.state_    = state;

   servers_.erase (self_.endpoint_);
   
   //! This ensures we aren't part of the quorum ourselves
   PAXOS_ASSERT (servers_.find (self_.endpoint_) == servers_.end ());
}

void
quorum::adjust_our_state (
   enum detail::remote_server::state state)
{
   self_.state_ = state;
}


struct quorum::self const &
quorum::self () const
{
   return self_;
}

void
quorum::add (
   boost::asio::ip::address const &     address,
   uint16_t                             port)
{
   detail::remote_server server;
   servers_[boost::asio::ip::tcp::endpoint (address, port)] = server;

   //! We can never add ourselves to the quorum
   PAXOS_ASSERT (servers_.find (self_.endpoint_) == servers_.end ());
}


void
quorum::reset_state ()
{
   for (map_type::value_type & i : servers_)
   {
      if (i.second.state () != detail::remote_server::state_dead)
      {
         i.second.set_state (detail::remote_server::state_non_participant);
      }
   }
}


bool
quorum::needs_new_leader () const
{
   uint16_t leader_count = 0;

   if (self_.state_ == detail::remote_server::state_leader)
   {
      ++leader_count;
   }

   for (map_type::value_type const & i : servers_)
   {
      switch (i.second.state ())
      {
            case detail::remote_server::state_unknown:
               /*!
                 Unable to determine whether we need a new leader, since we have not
                 contacted this host even once. This typically occurs right after the
                 start of a new server, when we haven't attempted to perform a handshake
                 with this host yet.
               */
               PAXOS_DEBUG ("remote server " << i.first << " has state unknown, "
                            "unable to determine whether we need a new leader")
               return false;

            case detail::remote_server::state_leader:
               ++leader_count;
               break;

            default:
               break;
      }
   }

   return leader_count != 1;
}


boost::asio::ip::tcp::endpoint
quorum::determine_leader () const
{
   //! Sort all the hosts by their id
   std::map <boost::uuids::uuid, boost::asio::ip::tcp::endpoint> host_ids;
   host_ids[self_.id_] = self_.endpoint_;

   for (map_type::value_type const & i : servers_)
   {
      if (i.second.state () == detail::remote_server::state_dead)       
      {
         //! Skipping dead nodes from participating in leader election
         continue;
      }
      host_ids[i.second.id ()] = i.first;
   }

   //! Always select the host with the highest id
   return host_ids.rbegin ()->second;
}

void
quorum::set_leader (
   boost::asio::ip::tcp::endpoint const &       endpoint)
{
   PAXOS_ASSERT (endpoint == determine_leader ());

   PAXOS_INFO ("setting new leader to " << endpoint);

   self_.state_ =
      (self_.endpoint_ == endpoint 
       ? detail::remote_server::state_leader
       : detail::remote_server::state_follower);

   for (map_type::value_type & i : servers_)
   {
      if (i.second.state () == detail::remote_server::state_dead)
      {
         continue;
      }

      i.second.set_state (
         (i.first == endpoint
          ? detail::remote_server::state_leader
          : detail::remote_server::state_follower));
   }
}


bool
quorum::we_are_the_leader () const
{
   return 
      needs_new_leader () == false
      && self_.state_ == detail::remote_server::state_leader;
}


detail::remote_server &
quorum::lookup (
   boost::asio::ip::tcp::endpoint const &    endpoint)
{
   auto pos = servers_.find (endpoint);

   PAXOS_ASSERT (pos != servers_.end ());

   return pos->second;
}

detail::remote_server const &
quorum::lookup (
   boost::asio::ip::tcp::endpoint const &    endpoint) const
{
   auto pos = servers_.find (endpoint);

   PAXOS_ASSERT (pos != servers_.end ());

   return pos->second;
}



};
