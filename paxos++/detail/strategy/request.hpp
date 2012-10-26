/*!
  Copyright (c) 2012, Leon Mergen, all rights reserved.
 */

#ifndef LIBPAXOS_CPP_DETAIL_STRATEGY_REQUEST_HPP
#define LIBPAXOS_CPP_DETAIL_STRATEGY_REQUEST_HPP

#include "../command.hpp"
#include "../tcp_connection_fwd.hpp"

namespace paxos { namespace detail {
class paxos_context;
}; };

namespace paxos { namespace detail { namespace quorum {
class server_view;
}; }; };

namespace paxos { namespace detail { namespace strategy {

/*!
  \brief Keeps track of context information required by the various Paxos protocol implementations
 */
struct request
{
   detail::tcp_connection_ptr    connection_;
   detail::command               command_;
   detail::quorum::server_view & quorum_;
   detail::paxos_context &       global_state_;
};

}; }; };

#endif //! LIBPAXOS_CPP_DETAIL_STRATEGY_REQUEST_HPP
