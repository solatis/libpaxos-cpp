/*!
  Copyright (c) 2012, Leon Mergen, all rights reserved.
 */

#ifndef LIBPAXOS_CPP_DETAIL_PROTOCOL_ANNOUNCE_LEADERSHIP_HPP
#define LIBPAXOS_CPP_DETAIL_PROTOCOL_ANNOUNCE_LEADERSHIP_HPP

#include <boost/asio/ip/basic_endpoint.hpp>

#include "../tcp_connection.hpp"

namespace paxos { namespace detail { namespace protocol {
class command;
class protocol;
}; }; };

namespace paxos { namespace detail { namespace protocol {

/*!
  \brief Connects to all nodes in quorum to announce that we are their leader
 */
class announce_leadership
{
private:

public:


   /*!
     \brief Constructor
     \param protocol    Reference to the protocol we live in
    */
    announce_leadership (
       detail::protocol::protocol &     protocol);

   /*!
     \brief Starts announce_leadership with all nodes in quorum
    */
   void
   start ();

   /*!
     \brief Called when protocol.cpp receives a connection from the leader
    */
   void
   receive_leader (
      tcp_connection::pointer   connection,
      command const &           command);

private:

   /*!
     \brief Attempts to connect with all nodes inside the quorum
    */
   void
   step1 ();

   /*!
     \brief Called when a new connection has been established
    */
   void
   step2 (
      boost::asio::ip::tcp::endpoint const &    endpoint,
      tcp_connection::pointer                   connection,
      boost::system::error_code const &         error);

   /*!
     \brief Called when a node receives a handshake request
    */
   void
   step3 (
      tcp_connection::pointer   connection,
      command const &           command);

private:

   protocol &                   protocol_;

};

}; }; };

#endif //! LIBPAXOS_CPP_DETAIL_PROTOCOL_ANNOUNCE_LEADERSHIP_HPP
