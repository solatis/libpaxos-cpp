/*!
  Copyright (c) 2012, Leon Mergen, all rights reserved.
 */

#ifndef LIBPAXOS_CPP_DETAIL_QUORUM_PROTOCOL_ANNOUNCE_LEADERSHIP_HPP
#define LIBPAXOS_CPP_DETAIL_QUORUM_PROTOCOL_ANNOUNCE_LEADERSHIP_HPP

#include <boost/asio/ip/tcp.hpp>

#include "../../tcp_connection_fwd.hpp"

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
  \brief Defines protocol used when a leader announces rulership over a quorum
 */
class announce_leadership
{
public:

   /*!
     \brief Announce our leadership to another host
     \param io_service  Interface to the OS'es underlying I/O functions
     \param connection  Connection to the host we're shaking hands with     
     \param quorum      Quorum to store the host's details in
    */
   static void
   step1 (
      boost::asio::io_service & io_service,
      tcp_connection_ptr   connection,
      detail::quorum::quorum &  quorum);

   /*!
     \brief Entry point when a announce_leadership has been received
    */
   static void
   step2 (
      tcp_connection_ptr   connection,
      detail::command const &   command,
      detail::quorum::quorum &  quorum);

private:   

};

}; }; }; };

#endif //! LIBPAXOS_CPP_DETAIL_QUORUM_PROTOCOL_ANNOUNCE_LEADERSHIP_HPP
