/*!
  Copyright (c) 2012, Leon Mergen, all rights reserved.
 */

#ifndef LIBPAXOS_CPP_DETAIL_REMOTE_SERVER_HPP
#define LIBPAXOS_CPP_DETAIL_REMOTE_SERVER_HPP

#include <boost/optional.hpp>

#include "tcp_connection.hpp"

namespace paxos { namespace detail {

/*!
  \brief Contains information about a remote paxos server.
 */
class remote_server
{
public:

   enum state
   {
      state_unknown,
      state_dead,
      state_non_participant,
      state_leader,
      state_follower,
      state_client
   };

   static std::string 
   to_string (
      enum state);

   /*!
     \brief Default constructor
     
     Initializes remote server with a default dead state.
    */
   remote_server ();

   /*!
     \brief Adjusts state of server
    */
   void
   set_state (
      enum state        state);

   /*!
     \brief Access to current state
    */
   enum state
   state () const;


   /*!
     \brief Sets the tcp connection we're currently connected to.
    */
   void
   set_connection (
      tcp_connection::pointer   connection);

   /*!
     \brief Returns true if connection_ is set.
    */
   bool
   has_connection ();

   tcp_connection &
   connection ();   

   tcp_connection const &
   connection () const;

private:

private:

   enum state                                   state_;
   boost::optional <tcp_connection::pointer>    connection_;
};

}; };

#endif //! LIBPAXOS_CPP_DETAIL_REMOTE_SERVER_HPP
