/*!
  Copyright (c) 2012, Leon Mergen, all rights reserved.
 */

#ifndef LIBPAXOS_CPP_DETAIL_QUORUM_QUORUM_HPP
#define LIBPAXOS_CPP_DETAIL_QUORUM_QUORUM_HPP

#include <map>
#include <vector>

#include <boost/noncopyable.hpp>
#include <boost/optional.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/deadline_timer.hpp>

#include "../../configuration.hpp"
#include "server.hpp"

namespace boost { namespace uuids {
struct uuid;
}; };

namespace paxos { namespace detail { namespace quorum {

/*!
  \brief Represents the quorum and provides interface to utility funcitons
 */
class quorum : private boost::noncopyable
{
public:   

   /*!
     \brief Constructor for clients
     \param io_service  References the OS'es underlying I/O services

     This constructor assumes we are not part of the quorum. 
    */
   quorum (
      boost::asio::io_service &                 io_service,
      paxos::configuration const &              configuration);

   /*
     \brief Constructor for servers
     \param io_service  References the OS'es underlying I/O services
     \param endpoint    Our endpoint so we can identify ourselves within the quorum

     This constructor assumes we are part of the quorum. This will automatically make
     sure our endpoint is added to the quorum.
    */
   quorum (
      boost::asio::io_service &                 io_service,
      boost::asio::ip::tcp::endpoint const &    endpoint,
      paxos::configuration const &              configuration);

   /*!
     \brief Resets our state completely
     
     This function is called when some quorum inconsistency is detected between multiple
     hosts, and this resets the state of all hosts to 'unknown', except our own entry,
     which is set to 'non_participant'.
    */
   void
   reset ();

   /*!
     \brief Called when a new leader is announced
     \param endpoint    Endpoint of the host that is the new leader
    */
   void
   new_leader (
      boost::asio::ip::tcp::endpoint const &    endpoint);

   /*!
     \brief Access to our own endpoint, so we can identify ourselves
     \pre we_have_our_endpoint () == true
    */
   boost::asio::ip::tcp::endpoint const &
   our_endpoint () const;

   /*!
     \brief Determines whether an endpoint was registered
    */
   bool
   we_have_our_endpoint () const;

   /*!
     \brief Based on our_endpoint (), looks up the state we currently have
    */
   enum server::state
   our_state () const;


   /*!
     \brief Based on endpoint, adjusts a server's state
    */
   void
   set_host_state (
      boost::asio::ip::tcp::endpoint const &    server,
      enum server::state                        state);

   /*!
     \brief Based on our_endpoint (), looks up our host's id
    */
   boost::uuids::uuid const &
   our_id () const;

   /*!
     \brief Based on endpoint, adjusts a server's unique identifier
    */
   void
   set_host_id (
      boost::asio::ip::tcp::endpoint const &    server,
      boost::uuids::uuid const &                id);


   /*!
     \brief Returns true if more than 50% of the servers in quorum are alive
    */
   bool
   has_majority (
      boost::asio::ip::tcp::endpoint const &    endpoint) const;

   /*!
     \brief Returns the endpoint of the host within our quorum we think should be a leader
    */
   boost::asio::ip::tcp::endpoint
   who_should_be_leader () const;

   /*!
     \brief Returns true if we have one leader in the quorum
    */
   bool
   we_have_a_leader () const;


   /*!
     \brief Returns true if we have one leader in the quorum and that leader 
            has a connection
    */
   bool
   we_have_a_leader_connection () const;

   /*!
     \brief Returns endpoint of the current leader of the quorum
     \pre we_have_a_leader () == true
    */
   boost::asio::ip::tcp::endpoint const &
   who_is_our_leader () const;


   /*!
     \brief Provides access to all connections to all "live" nodes
     
     This function is useful for when messages need to be broadcasted to all nodes within
     the quorum.
    */
   std::vector  <boost::asio::ip::tcp::endpoint>
   live_server_endpoints ();

   /*!
     \brief Basically a forward to heartbeat ()

     We can simply forward this to heartbeat since that will automatically establish
     connections / handshakes with hosts in recovery mode.
    */
   void
   bootstrap ();

   /*!
     \brief Adds new node to the quorum
    */
   void
   add (
      boost::asio::ip::tcp::endpoint const &    endpoint);

   /*!
     \brief Resets server to a 'dead' state 
    */
   void
   mark_dead (
      boost::asio::ip::tcp::endpoint const &    endpoint);      

   /*!
     \brief Called when a connection has been established with a server
    */
   void
   control_connection_established (
      boost::asio::ip::tcp::endpoint const &    endpoint,
      tcp_connection_ptr                        connection);


   /*!
     \brief Called when a connection has been established with a server
    */
   void
   broadcast_connection_established (
      boost::asio::ip::tcp::endpoint const &    endpoint,
      tcp_connection_ptr                        connection);

   /*!
     \brief Determines whether an endpoint is inside this quorum
    */
   bool
   has_server (
      boost::asio::ip::tcp::endpoint const &    endpoint) const;

   /*!
     \brief Access to a server
     \pre servers_.find (endpoint) != servers_.end ();
    */
   detail::quorum::server const &
   lookup_server (
      boost::asio::ip::tcp::endpoint const &    endpoint) const;

   /*!
     \brief Access to a server
     \pre servers_.find (endpoint) != servers_.end ();
    */
   detail::quorum::server &
   lookup_server (
      boost::asio::ip::tcp::endpoint const &    endpoint);

private:

   /*!
     \brief Returns true if the quorum is ready for a leader election

     This essentially returns true if all hosts have been either handshaked with, or marked
     as dead.
    */
   bool
   is_ready_for_leader_election () const;

   /*!
     \brief Ensures connections with all servers, performs handshakes and elects leader
    */
   void
   heartbeat ();   

   /*!
     \brief Ensures broken connections with servers are re-established
    */
   void
   heartbeat_validate_connections ();

   /*!
     \brief Ensures a handshake is made with all servers that have a connection
    */
   void
   heartbeat_handshake ();

   /*!
     \brief Ensures a leader is elected if required
    */
   void
   heartbeat_elect_leader ();

private:

   boost::asio::io_service &                                            io_service_;
   paxos::configuration                                                 configuration_;

   boost::optional <boost::asio::ip::tcp::endpoint>                     our_endpoint_;
   boost::asio::deadline_timer                                          heartbeat_timer_;

   std::map <boost::asio::ip::tcp::endpoint, detail::quorum::server>    servers_;

};

}; }; };

#endif //! LIBPAXOS_CPP_DETAIL_QUORUM_QUORUM_HPP
