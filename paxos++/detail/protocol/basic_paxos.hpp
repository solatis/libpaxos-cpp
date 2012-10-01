/*!
  Copyright (c) 2012, Leon Mergen, all rights reserved.
 */

#ifndef LIBPAXOS_CPP_DETAIL_PROTOCOL_BASIC_PAXOS_HPP
#define LIBPAXOS_CPP_DETAIL_PROTOCOL_BASIC_PAXOS_HPP

#include <stdint.h>

#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio/ip/basic_endpoint.hpp>

#include "../../error.hpp"
#include "../tcp_connection.hpp"

namespace paxos { namespace detail { namespace protocol {
class command;
class protocol;
}; }; };

namespace paxos { namespace detail { namespace protocol {

/*!
  \brief Implementation of the basic_paxos algorithm
 */
class basic_paxos
{
private:
   enum response
   {
      response_none,
      response_ack,
      response_reject
   };

   struct state
   {
      std::map <boost::asio::ip::tcp::endpoint, enum response>  accepted;
      std::map <boost::asio::ip::tcp::endpoint, std::string>    responses;
   };

public:

   typedef boost::function <void (boost::optional <enum paxos::error_code>,
                                  std::string const &)>                         client_callback_type;

public:
   /*!
     \brief Constructor
     \param protocol    Reference to the protocol we live in
    */
    basic_paxos (
       detail::protocol::protocol &     protocol);

   /*!
     \brief Starts basic_paxos with all nodes in quorum
     \param client_connection   Connection with the client that initiated the request
     \param command             Command received by the client
     \param callback            Callback we can override
    */
   void
   start (
      tcp_connection::pointer           client_connection,
      command const &                   command);


   /*!
     \brief Received by a follower when a leader wants to prepare a request
     \param connection          Connection with the leader
     \param command             Command received by the follower
    */
   void
   receive_prepare (
      tcp_connection::pointer           connection,
      command const &                   command);


   /*!
     \brief Received by a follower when a leader has sent an accept
     \param connection          Connection with the leader
     \param command             Command received by the follower
    */
   void
   receive_accept (
      tcp_connection::pointer           connection,
      command const &                   command);

private:

private:

   /*!
     \brief Sends prepare command to server as defined in Phase 1a of Paxos protocol
     \param client_connection   Connection to the client that initiated the request
     \param server_endpoint     Endpoint to the server we're sending the command to
     \param server_connection   Connection to the server that we will send the prepare to
     \param byte_array          Workload sent by the client
     \param callback            Callback we can override
    */
   void
   send_prepare (
      tcp_connection::pointer                   client_connection,
      boost::asio::ip::tcp::endpoint const &    server_endpoint,
      tcp_connection::pointer                   server_connection,
      std::string const &                       byte_array,
      boost::shared_ptr <struct state>          state);


   /*!
     \brief Received by leader after it has sent a prepare request to all followers
    */
   void
   receive_promise (
      tcp_connection::pointer                   client_connection,
      boost::asio::ip::tcp::endpoint const &    server_endpoint,
      tcp_connection::pointer                   server_connection,
      std::string                               byte_array,
      command const &                           command,
      boost::shared_ptr <struct state>          state);

   /*!
     \brief Sent to a server after all servers in a quorum have promised a request
    */
   void
   send_accept (
      tcp_connection::pointer                   client_connection,
      boost::asio::ip::tcp::endpoint const &    server_endpoint,
      tcp_connection::pointer                   server_connection,
      std::string const &                       byte_array,
      boost::shared_ptr <struct state>          state);


   void
   receive_accepted (
      tcp_connection::pointer                   client_connection,
      boost::asio::ip::tcp::endpoint const &    server_endpoint,
      command const &                           command,
      boost::shared_ptr <struct state>          state);


private:

   protocol &                   protocol_;

   /*!
     \brief Keeps track of the highest proposal id seen so far
    */
   uint64_t                     proposal_id_;

};

}; }; };

#endif //! LIBPAXOS_CPP_DETAIL_PROTOCOL_BASIC_PAXOS_HPP
