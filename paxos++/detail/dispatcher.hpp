/*!
  Copyright (c) 2012, Leon Mergen, all rights reserved.
 */

#ifndef LIBPAXOS_CPP_DETAIL_DISPATCHER_HPP
#define LIBPAXOS_CPP_DETAIL_DISPATCHER_HPP

#include "tcp_connection.hpp"

namespace paxos { namespace detail {
class command;
class paxos_state;
}; };

namespace paxos { namespace detail { namespace quorum {
class quorum;
}; }; };

namespace paxos { namespace detail {

/*!
  \brief Ensures that commands are dispatched to the proper handler
 */
class dispatcher
{
public:

   /*!
     \brief Dispatches command to the appropriate handler
     \param connection  Connection that received this command
     \param command     The command that was received
     \param quorum      Reference to our server's current quorum
     \param state       Keeps track of state information for various paxos strategies
    */
   static void
   dispatch_command (
      tcp_connection::pointer   connection,
      detail::command const &   command,
      detail::quorum::quorum &  quorum,
      detail::paxos_state &     state);

private:


};

}; };

#endif //! LIBPAXOS_CPP_DETAIL_DISPATCHER_HPP
