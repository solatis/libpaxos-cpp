#include <functional>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "protocol/announce_leadership.hpp"
#include "protocol/establish_connection.hpp"
#include "protocol/handshake.hpp"

#include "../util/debug.hpp"
#include "../../configuration.hpp"
#include "quorum.hpp"

namespace paxos { namespace detail { namespace quorum {

quorum::quorum (
   boost::asio::io_service &    io_service,
   paxos::configuration const & configuration)
   : io_service_ (io_service),
     configuration_ (configuration),
     heartbeat_timer_ (io_service)
{
   /*!
     This is the client constructor, and we do not have to initialize anything here,
     since we are not a part of the quorum.
    */
}

quorum::quorum (
   boost::asio::io_service &                    io_service,
   boost::asio::ip::tcp::endpoint const &       endpoint,
   paxos::configuration const &                 configuration)
   : io_service_ (io_service),
     configuration_ (configuration),
     our_endpoint_ (endpoint),
     heartbeat_timer_ (io_service)
{
   this->add (endpoint);

   /*!
     Since this is the constructor for a server, we need to initialize our own
     entry inside the quorum, which consists of two things:

     * to initialize a UUID for our host, which we will communicate to other hosts inside
       the quorum and use to decide who should be the leader of the quorum.

     * mark our own state as "non-participant", instead of the default of "unknown". This may
       seem a small step, but it is important: this will cause our state to other hosts to be
       propagated as "non participant" instead of "unknown", and is used by the internal function
       is_ready_for_leader_election () to determine whether we have handshaked with all hosts
       at least once.
    */
   boost::uuids::basic_random_generator <boost::mt19937> gen;

   set_host_id    (endpoint, gen ());
   set_host_state (endpoint, server::state_non_participant);
}

void
quorum::reset ()
{
   PAXOS_WARN ("resetting quorum, inconsistency detected!");

   for (auto & i : servers_)    
   {
      if (i.first == our_endpoint ())
      {
         i.second.set_state (server::state_non_participant);
      }
      else
      {
         i.second.set_state (server::state_unknown);
      }

      i.second.reset_connection ();
   }
}

void
quorum::new_leader (
   boost::asio::ip::tcp::endpoint const &    endpoint)
{
   if (endpoint != our_endpoint ())
   {
      /*! 
        In this case, we are a follower, so make sure we mark ourselves as such, so that
        that state is propagated with the next handshake.
      */
      lookup_server (our_endpoint ()).set_state (server::state_follower);
   }

   PAXOS_INFO ("marking " << endpoint << " as leader, our endpoint = " << our_endpoint ());

   lookup_server (endpoint).set_state (server::state_leader);
}



boost::asio::ip::tcp::endpoint const &
quorum::our_endpoint () const
{
   PAXOS_ASSERT (we_have_our_endpoint () == true);
   return *our_endpoint_;
}

bool
quorum::we_have_our_endpoint () const
{
   return our_endpoint_.is_initialized () == true;
}

enum server::state
quorum::our_state () const
{
   if (we_have_our_endpoint () == false)
   {
      PAXOS_DEBUG ("quorum is a CLIENT");
      return server::state_client;
   }

   PAXOS_DEBUG ("quorum is a SERVER, our_state = " << server::to_string (lookup_server (our_endpoint ()).state ()));

   return lookup_server (our_endpoint ()).state ();
}

void
quorum::set_host_state (
   boost::asio::ip::tcp::endpoint const &       server,
   enum server::state                           state)
{
   PAXOS_DEBUG ("setting state of host " << server << " to state = " << server::to_string (state));
   lookup_server (server).set_state (state);
}


boost::uuids::uuid const &
quorum::our_id () const
{
   PAXOS_DEBUG ("our_id () == " << lookup_server (our_endpoint ()).id ());
   return lookup_server (our_endpoint ()).id ();
}

void
quorum::set_host_id (
   boost::asio::ip::tcp::endpoint const &    server,
   boost::uuids::uuid const &                id)
{
   PAXOS_DEBUG ("setting host id of server = " << server << " to id = " << id);
   lookup_server (server).set_id (id);
}


void
quorum::add (
   boost::asio::ip::tcp::endpoint const &    endpoint)
{
   if (servers_.find (endpoint) == servers_.end ())
   {
      servers_.insert (std::make_pair (endpoint,
                                       server (io_service_,
                                               endpoint)));
   }
}

void
quorum::mark_dead (
   boost::asio::ip::tcp::endpoint const &    endpoint)
{
   if (has_server (endpoint) == false)
   {
      PAXOS_DEBUG ("Endpoint " << endpoint << " not part of quorum, not marking dead");
      return;
   }

   PAXOS_DEBUG ("Connection error with " << endpoint << ", setting state to dead");

   server & server = lookup_server (endpoint);
   server.reset_connection ();
   server.set_state (server::state_dead);
   server.set_id (boost::uuids::uuid ());

   PAXOS_ASSERT (server.has_connection () == false);
}

void
quorum::broadcast_connection_established (
   boost::asio::ip::tcp::endpoint const &       endpoint,
   tcp_connection_ptr                           connection)
{
   PAXOS_DEBUG ("Established broadcast connection with " << endpoint);

   server & server = lookup_server (endpoint);
   server.set_broadcast_connection (connection);
}

void
quorum::control_connection_established (
   boost::asio::ip::tcp::endpoint const &       endpoint,
   tcp_connection_ptr                           connection)
{
   PAXOS_DEBUG ("Established control connection with " << endpoint);

   server & server = lookup_server (endpoint);
   server.set_control_connection (connection);
}

bool
quorum::has_server (
   boost::asio::ip::tcp::endpoint const &       endpoint) const
{
   return servers_.find (endpoint) != servers_.end ();
}


detail::quorum::server const &
quorum::lookup_server (
   boost::asio::ip::tcp::endpoint const &       endpoint) const
{
   auto pos = servers_.find (endpoint);
   
   PAXOS_ASSERT (pos != servers_.end ());

   return pos->second;
}

detail::quorum::server &
quorum::lookup_server (
   boost::asio::ip::tcp::endpoint const &       endpoint)
{
   auto pos = servers_.find (endpoint);
   
   PAXOS_ASSERT (pos != servers_.end ());

   return pos->second;
}


bool
quorum::is_ready_for_leader_election () const
{
   for (auto const & i : servers_)
   {
      if (i.second.state () == server::state_unknown)
      {
         PAXOS_WARN ("server " << i.first << " has state_unknown!");
         return false;
      }
      else if (i.second.state () == server::state_non_participant
               && i.second.has_connection ()== false)
      {
         /*!
           This will take one handshake to recover, either the node will be set in a
           DEAD state or the connection will be established.
         */
         PAXOS_WARN ("server " << i.first << " has state_non_participant and no connection");
         return false;
      }
   }

   PAXOS_INFO ("we are ready for leader election!");

   return true;
}

bool
quorum::has_majority (
   boost::asio::ip::tcp::endpoint const &       endpoint) const
{
   server const & server = lookup_server (endpoint);

   return 
      static_cast <double> (server.live_servers ().size ()) 
      >= (static_cast <double> (servers_.size ()) * configuration_.quorum_majority_factor ());
}

boost::asio::ip::tcp::endpoint
quorum::who_should_be_leader () const
{
   std::map <boost::uuids::uuid, boost::asio::ip::tcp::endpoint> host_ids;

   for (auto const & i : servers_)
   {
      host_ids[i.second.id ()] = i.first;
   }

   /*!
     If this check fails, we have multiple servers with the same id.
    */
   PAXOS_ASSERT (host_ids.size () == servers_.size ());
   
   return host_ids.rbegin ()->second;
}

bool
quorum::we_have_a_leader () const
{
   uint16_t leader_count = 0;

   for (auto const & i : servers_)
   {
      if (i.second.state () == server::state_leader)
      {
         ++leader_count;
      }
   }

   return leader_count == 1;
}

bool
quorum::we_have_a_leader_connection () const
{
   return 
      this->we_have_a_leader () == true
      && this->lookup_server (this->who_is_our_leader ()).has_connection () == true;
}


boost::asio::ip::tcp::endpoint const &
quorum::who_is_our_leader () const
{
   PAXOS_ASSERT (we_have_a_leader () == true);

   for (auto const & i : servers_)
   {
      if (i.second.state () == server::state_leader)
      {
         return i.first;
      }
   }
   
   PAXOS_UNREACHABLE ();
}

std::vector  <boost::asio::ip::tcp::endpoint>
quorum::live_server_endpoints ()
{
   std::vector <boost::asio::ip::tcp::endpoint> servers;

   for (auto const & i : servers_)
   {
      switch (i.second.state ())
      {
            case server::state_unknown:
            case server::state_dead:
            case server::state_non_participant:
               PAXOS_DEBUG ("skipping dead from live servers: " << i.first);
               continue;

            case server::state_follower:
            case server::state_leader:
               PAXOS_DEBUG ("adding to live servers: " << i.first);
               servers.push_back (i.first);
               break;

            case server::state_client:
               /*! 
                 Should never be part of the quorum's registered servers
               */
               PAXOS_UNREACHABLE ();
      };
   }

   return servers;
}


void
quorum::bootstrap ()
{
   heartbeat ();
}


void
quorum::heartbeat ()
{
   PAXOS_DEBUG ("starting hearbeat");

   /*!
     Let's first ensure we have connections to all nodes inside the quorum
    */
   heartbeat_validate_connections ();


   /*!
     Perform a handshake with all servers we have a connection with.
    */
   heartbeat_handshake ();


   /*!
     Elects a leader if required
    */
   heartbeat_elect_leader ();

   /*!
     And ensure that we're called again
    */
   heartbeat_timer_.expires_from_now (
      boost::posix_time::milliseconds (configuration_.heartbeat_interval ()));
   heartbeat_timer_.async_wait (
      std::bind (&quorum::heartbeat, this));
}


void
quorum::heartbeat_validate_connections ()
{
   PAXOS_DEBUG ("validating connections");

   for (auto & i : servers_)
   {
      if (i.second.has_connection () == false)
      {
         protocol::establish_connection::step1 (io_service_,
                                                i.first,
                                                *this);
      }
   }
}


void
quorum::heartbeat_handshake ()
{
   PAXOS_DEBUG ("handshaking with nodes");

   for (auto & i : servers_)
   {
      if (i.second.has_connection () == false)
      {
         continue;
      }
      
      if (we_have_our_endpoint () == true
          && i.first == our_endpoint ())
      {
         /*!
           We should skip ourselves in handshakes, first of all, because it doesn't make
           sense, and secondly, it can cause trouble where we cannot adjust our own state
           because it keeps getting handshaked back to 'non-participant'.
          */
         continue;
      }

      PAXOS_DEBUG ("connection " << i.first << " is valid, have id = " << i.second.id () << ", performing handshake");
      protocol::handshake::step1 (io_service_,
                                  i.first,
                                  i.second.control_connection (),
                                  *this);
   }
}


void
quorum::heartbeat_elect_leader ()
{
   if (our_state () == server::state_client)
   {
      /*!
        We are a client, never elect leader.
       */
      return;
   }

   if (this->is_ready_for_leader_election () == true
       && this->who_should_be_leader () == this->our_endpoint ())
   {
      for (auto & i : servers_)
      {
         if (i.second.state () == server::state_dead)
         {
            //! Skip all dead servers
            continue;
         }

         PAXOS_ASSERT (i.second.has_connection () == true);

         PAXOS_DEBUG ("announcing leadership to server with state = " << server::to_string (i.second.state ()));

         protocol::announce_leadership::step1 (io_service_,
                                               i.second.control_connection (),
                                               *this);
      }
   }
}

}; }; };
