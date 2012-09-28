/*!
  Copyright (c) 2012, Leon Mergen, all rights reserved.
 */

#ifndef LIBPAXOS_CPP_DETAIL_QUORUM_HPP
#define LIBPAXOS_CPP_DETAIL_QUORUM_HPP

#include <set>
#include <string>

#include <boost/bind.hpp>
#include <boost/optional.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/asio/ip/address.hpp>

#include "detail/remote_server.hpp"

namespace paxos { 

/*!
  \brief Quorum of servers in the pool

  The quorum maintains a state of all the servers inside the pool. The assumption is
  that the quorum is fixed and is equal for all servers in the pool: that is, it is up
  to the application to ensure that this configuration is the same on all hosts.

  Please be aware that the quorum does not necessarily describe /all/ connections to all
  other servers. It only describes the long-lived connections required by the Paxos protocol, 
  and not any short-lived connections required by leader election, for example.

  Note that there are several options here:
   * we are a leader, in which case we (should) have connections to all other followers;
   * we are a follower, in which case we only have a connection to our leader;
   * we are a client, in which we can have several connections to anyone.

  If these conditions are not met, an assertion is thrown.
 */
class quorum
{
   typedef std::map <boost::asio::ip::tcp::endpoint, detail::remote_server> map_type;

public:

   struct self
   {
      boost::asio::ip::tcp::endpoint    endpoint_;
      boost::uuids::uuid                id_;
      enum detail::remote_server::state state_;
   };

   /*!
     \brief Denotes which server we are
     \param address     Our local IP address
     \param port        Port we're listening at
     \param id          Our unique identification number within the quorum
     \param state       Our state we will assume by default
     \pre The server is part of the quorum
    */
   void
   we_are (
      boost::asio::ip::address const &  address,
      uint16_t                          port,
      boost::uuids::uuid const &        id,
      enum detail::remote_server::state state = detail::remote_server::state_non_participant);

   /*!
     Adjusts our current state
    */
   void
   adjust_our_state (
      enum detail::remote_server::state state);

   /*!
     \brief Access to the endpoint we represent
    */
   struct self const &
   self () const;

   /*!
     \brief Adds a new server to the quorum with a default 'dead' state
    */
   void
   add (
      boost::asio::ip::address const &  address,
      uint16_t                          port);


   /*!
     \brief Ensures all nodes in the quorum that are not dead are set to state_non_participant
    */
   void
   reset_state ();

   /*!
     \brief Determines whether the quorum currently needs a new leader
     \returns Returns true if a new leader is required

     This essentially returns true if we have contacted all the hosts at least
     once, and the amount of hosts in our quorum that think they are the leader
     is not exactly 1. 
    */
   bool
   needs_new_leader () const;

   /*!
     \brief Based on the id's of all the hosts in the quorum we known, determines leader
    */
   boost::asio::ip::tcp::endpoint
   determine_leader () const;

   /*!
     \brief Sets the new leader of this quorum
    */
   void
   set_leader (
      boost::asio::ip::tcp::endpoint const &    endpoint);

   /*!
     \brief Determines whether self () is the current leader of the quorum
    */
   bool
   we_are_the_leader () const;
   

   detail::remote_server &
   lookup (
      boost::asio::ip::tcp::endpoint const &    endpoint);

   detail::remote_server const &
   lookup (
      boost::asio::ip::tcp::endpoint const &    endpoint) const;

   map_type &
   servers ();

   map_type const &
   servers () const;

private:

   struct self  self_;

   map_type                             servers_;

};

};

#include "quorum.inl"

#endif //! LIBPAXOS_CPP_DETAIL_QUORUM_HPP
