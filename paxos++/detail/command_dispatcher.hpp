/*!
  Copyright (c) 2012, Leon Mergen, all rights reserved.
 */

#ifndef LIBPAXOS_CPP_DETAIL_COMMAND_DISPATCHER_HPP
#define LIBPAXOS_CPP_DETAIL_COMMAND_DISPATCHER_HPP

#include <boost/optional.hpp>

#include "error.hpp"
#include "tcp_connection_fwd.hpp"

namespace paxos { namespace detail {
class command;
class paxos_context;
}; };

namespace paxos { namespace detail { namespace quorum {
class server_view;
}; }; };

namespace paxos { namespace detail {

/*!
  \brief Ensures that commands are dispatched to the proper handler

  In our traffic flow, we support the pipelining of multiple commands simultaneously over the same
  connection. This means that we need to do some kind of state tracking: replies to commands do not
  necessarily have to be received in the same order as they arrived.

  This class keeps track of this state. It ensures every command is associated with an id, and
  if a reply to a specific command is expected, waits until this reply is given (or a timeout).
 */
class command_dispatcher     
{
public:

   /*!
     \brief Dispatches stateless command to the appropriate handler
     \param connection  Connection that received this command
     \param command     The command that was received
     \param quorum      Reference to our server's current quorum
     \param state       Keeps track of state information for various paxos strategies
    */
   static void
   dispatch_command (
      boost::optional <enum error_code> error,
      tcp_connection_ptr                connection,
      detail::command const &           command,
      detail::quorum::server_view &     quorum,
      detail::paxos_context &           state);

private:
};

}; };

#endif //! LIBPAXOS_CPP_DETAIL_COMMAND_DISPATCHER_HPP
