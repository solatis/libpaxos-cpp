/*!
  Copyright (c) 2012, Leon Mergen, all rights reserved.
 */

#ifndef LIBPAXOS_CPP_DETAIL_TCP_CONNECTION_FWD_HPP
#define LIBPAXOS_CPP_DETAIL_TCP_CONNECTION_FWD_HPP

#include <boost/shared_ptr.hpp>

namespace paxos { namespace detail { 

class   tcp_connection;
typedef boost::shared_ptr <tcp_connection> tcp_connection_ptr;

}; };

#endif //! LIBPAXOS_CPP_DETAIL_TCP_CONNECTION_FWD_HPP
