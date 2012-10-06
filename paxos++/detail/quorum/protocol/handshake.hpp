/*!
  Copyright (c) 2012, Leon Mergen, all rights reserved.
 */

#ifndef LIBPAXOS_CPP_DETAIL_QUORUM_PROTOCOL_HANDSHAKE_HPP
#define LIBPAXOS_CPP_DETAIL_QUORUM_PROTOCOL_HANDSHAKE_HPP

#include <boost/asio/ip/tcp.hpp>

#include "../../connection/scoped_connection.hpp"

namespace boost { namespace asio {
class io_service;
}; };

namespace paxos { namespace detail { 
class command;
}; }; 

namespace paxos { namespace detail { namespace quorum { 
class quorum;
}; }; };

namespace paxos { namespace detail { namespace quorum { namespace protocol {

/*!
  \brief Defines protocol for handshaking between nodes in a quorum
 */
class handshake
{
public:

   /*!
     \brief Performs handshake with a specific host
     \param io_service  Interface to the OS'es underlying I/O functions
     \param connection  Connection to the host we're shaking hands with     
     \param quorum      Quorum to store the host's details in
    */
   static void
   step1 (
      boost::asio::io_service &                 io_service,
      connection::scoped_connection::pointer    connection,
      detail::quorum::quorum &                  quorum);

   /*!
     \brief Entry point when a handshake has been received
    */
   static void
   step2 (
      connection::tcp_connection::pointer       connection,
      detail::command const &                   command,
      detail::quorum::quorum &                  quorum);


private:   

};

}; }; }; };

#endif //! LIBPAXOS_CPP_DETAIL_QUORUM_PROTOCOL_HANDSHAKE_HPP
