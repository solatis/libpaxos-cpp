/*!
  Copyright (c) 2012, Leon Mergen, all rights reserved.
 */

#ifndef LIBPAXOS_CPP_DETAIL_PAXOS_REQUEST_HPP
#define LIBPAXOS_CPP_DETAIL_PAXOS_REQUEST_HPP

#include "tcp_connection_fwd.hpp"

#include "command.hpp"

namespace paxos { namespace detail {
class paxos_context;
}; };

namespace paxos { namespace detail { namespace quorum {
class quorum;
}; }; };

namespace paxos { namespace detail {

/*!
  \brief Keeps track of context information required by the various Paxos protocol implementations
 */
struct paxos_request
{
   tcp_connection_ptr           client_connection_;
   command                      command_;
   quorum::quorum &             quorum_;
   paxos_context &              global_state_;
};

}; };

#endif //! LIBPAXOS_CPP_DETAIL_PAXOS_REQUEST_HPP
