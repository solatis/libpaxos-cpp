/*!
  Copyright (c) 2012, Leon Mergen, all rights reserved.
 */

#ifndef LIBPAXOS_CPP_DETAIL_CLIENT_PROTOCOL_INITIATE_REQUEST_HPP
#define LIBPAXOS_CPP_DETAIL_CLIENT_PROTOCOL_INITIATE_REQUEST_HPP

#include <string>

#include <boost/function.hpp>
#include <boost/optional.hpp>

#include "../../../error.hpp"
#include "../../../exception/exception.hpp"
#include "../../tcp_connection.hpp"

namespace paxos { namespace detail { namespace quorum { 
class quorum;
}; }; };

namespace paxos { namespace detail { namespace client { namespace protocol {

/*!
  \brief Defines protocol used when a client sends a request to a leader
 */
class initiate_request
{
public:
   typedef boost::function <void (boost::optional <enum paxos::error_code>,
                                  std::string const &)>                         callback_type;

   /*!
     \brief Send request to leader
     \param byte_array  Binary data that holds the request
     \param quorum      Quorum that contains all information
     \param callback    Callback where results are stored
     \throws exception::not_ready Thrown when the quorum doesn't have a leader yet
    */
   static void
   step1 (      
      std::string const &       byte_array,
      detail::quorum::quorum &  quorum,
      callback_type             callback) throw (exception::not_ready);

private:   

};

}; }; }; };

#endif //! LIBPAXOS_CPP_DETAIL_CLIENT_PROTOCOL_INITIATE_REQUEST_HPP
