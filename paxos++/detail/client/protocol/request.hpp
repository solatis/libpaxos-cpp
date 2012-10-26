/*!
  Copyright (c) 2012, Leon Mergen, all rights reserved.
 */

#ifndef LIBPAXOS_CPP_DETAIL_CLIENT_PROTOCOL_REQUEST_HPP
#define LIBPAXOS_CPP_DETAIL_CLIENT_PROTOCOL_REQUEST_HPP

#include <string>

#include <boost/function.hpp>
#include <boost/optional.hpp>

#include "../../error.hpp"

namespace paxos { namespace detail { namespace quorum {
class client_view;
}; }; };

namespace paxos { namespace detail { namespace client { namespace protocol {

/*!
  \brief Keeps track of context information required by the various Paxos protocol implementations
 */
struct request
{
   std::string                                                                                  byte_array_;
   detail::quorum::client_view &                                                                quorum_;
   boost::function <void (boost::optional <enum detail::error_code>, std::string const &)>      callback_;

};

}; }; }; };

#endif //! LIBPAXOS_CPP_DETAIL_CLIENT_PROTOCOL_REQUEST_HPP
