/*!
  Copyright (c) 2012, Leon Mergen, all rights reserved.
 */

#ifndef LIBPAXOS_CPP_DETAIL_PROTOCOL_ELECT_LEADER_HPP
#define LIBPAXOS_CPP_DETAIL_PROTOCOL_ELECT_LEADER_HPP

#include <boost/asio/ip/basic_endpoint.hpp>

namespace paxos { namespace detail { namespace protocol {
class protocol;
}; }; };

namespace paxos { namespace detail { namespace protocol {

/*!
  \brief Pings all servers in quorum and determine the roles they think they have
 */
class elect_leader
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
      std::map <boost::asio::ip::tcp::endpoint, enum response>   responses;
   };

public:


   /*!
     \brief Constructor
     \param protocol    Reference to the protocol we live in
    */
    elect_leader (
       detail::protocol::protocol &     protocol);

   /*!
     \brief Starts elect_leader with all nodes in quorum
    */
   void
   start ();

   /*!
     \brief Called when protocol.cpp receives a new leadership claim
    */
   void
   receive_leader_claim (
      tcp_connection &  connection,
      command const &   command);

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
      tcp_connection &                          connection,
      boost::system::error_code const &         error,
      boost::shared_ptr <struct state>          state);

   /*!
     \brief Called when a node receives a handshake request
    */
   void
   step3 (
      tcp_connection &  connection,
      command const &   command);

   /*!
     \brief Called when a node has responded to a handshake request
    */
   void
   step4 (
      boost::asio::ip::tcp::endpoint const &    endpoint,
      tcp_connection &                          connection,
      command const &                           command,
      boost::shared_ptr <struct state>          state);

private:

   protocol &                   protocol_;

};

}; }; };

#endif //! LIBPAXOS_CPP_DETAIL_PROTOCOL_ELECT_LEADER_HPP
