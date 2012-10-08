/*!
  Copyright (c) 2012, Leon Mergen, all rights reserved.
 */

#ifndef LIBPAXOS_CPP_DETAIL_QUORUM_SERVER_HPP
#define LIBPAXOS_CPP_DETAIL_QUORUM_SERVER_HPP

#include <boost/uuid/uuid.hpp>

#include <boost/optional.hpp>
#include <boost/asio/ip/tcp.hpp>

#include "../tcp_connection.hpp"

namespace paxos { namespace detail { namespace quorum {

/*!
  \brief Represents a server within a quorum
 */
class server
{
public:

   enum state
   {
      /*! 
        When a server is first created, it's in an unknown state
      */
      state_unknown,

      /*! 
        Used when a connection to the server cannot be established or was lost
      */
      state_dead,

      /*! 
        Used when a connection to a server could be established, but 
        handshake hasn't occured yet.
      */
      state_non_participant,

      /*!
        When a server is known to be the leader of a quorum
       */
      state_leader,

      /*!
        When a server is known to be a follower in a quorum
       */
      state_follower,

      /*!
        Used to identify a client connection. This should *never* exist within a 
        quorum.
       */
      state_client
   };

   static std::string 
   to_string (
      enum state);
public:

   server (
      boost::asio::io_service &                 io_service,
      boost::asio::ip::tcp::endpoint const &    endpoint);

   /*!
     \brief Returns endpoint this server listens at
    */
   boost::asio::ip::tcp::endpoint const &
   endpoint () const;


   /*!
     \brief Access to the current state of the server
    */
   enum state
   state () const;

   /*!
     \brief Adjusts the current state of the server
    */
   void
   set_state (
      enum state        state);

   /*!
     \brief Access to the unique identifier of the server
    */
   boost::uuids::uuid const &
   id () const;

   /*!
     \brief Adjusts unique identifier of the server
    */
   void
   set_id (
      boost::uuids::uuid const &        id);

   
   /*!
     \brief Returns true if connection_ is set
    */
   bool
   has_connection () const;

   /*!
     \brief Sets a connection
    */
   void
   set_connection (
      detail::tcp_connection::pointer   connection);

   /*!
     \brief Resets the connection to a nullptr
    */
   void
   reset_connection ();

   /*!
     \brief Access to the underlying connection
     \pre has_connection () == true
    */
      detail::tcp_connection::pointer
   connection ();

private:

   boost::asio::ip::tcp::endpoint                       endpoint_;
   enum state                                           state_;
   boost::uuids::uuid                                   id_;

   boost::optional <detail::tcp_connection::pointer>    connection_;

};

}; }; };

#endif //! LIBPAXOS_CPP_DETAIL_QUORUM_SERVER_HPP
