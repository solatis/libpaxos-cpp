/*!
  Copyright (c) 2012, Leon Mergen, all rights reserved.
 */

#ifndef LIBPAXOS_CPP_DETAIL_PROTOCOL_HANDSHAKE_HPP
#define LIBPAXOS_CPP_DETAIL_PROTOCOL_HANDSHAKE_HPP

namespace paxos { namespace detail { namespace protocol {
class command;
class protocol;
}; }; };

namespace paxos { namespace detail { namespace protocol {

/*!
  \brief Pings all servers in quorum and determine the roles they think they have
 */
class handshake
{
public:

   /*!
     \brief Constructor
     \param protocol    Reference to the protocol we live in
    */
    handshake (
       detail::protocol::protocol &     protocol);

   /*!
     \brief Starts handshake with all nodes in quorum
    */
   void
   start ();


   void
   receive_handshake_start (
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
     \param host_endpoint Endpoint of the host we're communicating with, as defined in the quorum
     \param connection    The connection with the remote host
     \param error         Error that occurred if any
    */
   void
   step2 (
      boost::asio::ip::tcp::endpoint const &    endpoint,
      tcp_connection &                          connection,
      boost::system::error_code const &         error);

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
      command const &                           command);

private:

   protocol &                   protocol_;

};

}; }; };

#endif //! LIBPAXOS_CPP_DETAIL_PROTOCOL_HANDSHAKE_HPP
