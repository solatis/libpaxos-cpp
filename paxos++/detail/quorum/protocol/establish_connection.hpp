/*!
  Copyright (c) 2012, Leon Mergen, all rights reserved.
 */

#ifndef LIBPAXOS_CPP_DETAIL_QUORUM_PROTOCOL_ESTABLISH_CONNECTION_HPP
#define LIBPAXOS_CPP_DETAIL_QUORUM_PROTOCOL_ESTABLISH_CONNECTION_HPP

#include <boost/asio/ip/tcp.hpp>

#include "../../tcp_connection_fwd.hpp"

namespace boost { namespace asio {
class io_service;
}; };

namespace paxos {
class configuration;
};

namespace paxos { namespace detail { namespace quorum { 
class quorum;
}; }; };

namespace paxos { namespace detail { namespace quorum { namespace protocol {

/*!
  \brief Defines protocol for establishing connections
 */
class establish_connection
{
public:

   /*!
     \brief Establish connection with a certain host
     \param io_service  Interface to the OS'es underlying I/O functions
     \param endpoint    Endpoint of the host to establish connection with
     \param quorum      Quorum to store the connection within
    */
   static void
   step1 (
      boost::asio::io_service &                 io_service,
      boost::asio::ip::tcp::endpoint const &    endpoint,
      detail::quorum::quorum &                  quorum);

private:

   /*!
     \brief Callback for when a connection has been estblished
    */
   static void
   step2 (
      boost::system::error_code const &         error,
      boost::asio::ip::tcp::endpoint const &    endpoint,
      tcp_connection_ptr                        connection,
      detail::quorum::quorum &                  quorum);


private:   

};

}; }; }; };

#endif //! LIBPAXOS_CPP_DETAIL_QUORUM_PROTOCOL_ESTABLISH_CONNECTION_HPP
