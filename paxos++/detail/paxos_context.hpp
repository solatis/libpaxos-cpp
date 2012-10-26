/*!
  Copyright (c) 2012, Leon Mergen, all rights reserved.
 */

#ifndef LIBPAXOS_CPP_DETAIL_PAXOS_CONTEXT_HPP
#define LIBPAXOS_CPP_DETAIL_PAXOS_CONTEXT_HPP

#include <stdint.h>
#include <string>

#include <boost/function.hpp>

#include "strategy/request.hpp"
#include "request_queue/queue.hpp"

namespace paxos { 
class configuration;
};

namespace paxos { namespace detail { namespace strategy {
class strategy;
}; }; };


namespace paxos { namespace detail {

/*!
  \brief Keeps track of context information required by the various Paxos protocol implementations
 */
class paxos_context : private boost::noncopyable
{
public:

   typedef boost::function <std::string (int64_t, std::string const &)>  processor_type;

public:

   paxos_context (
      processor_type const &    processor,
      paxos::configuration &    configuration);

   ~paxos_context ();

   processor_type const &
   processor () const;

   detail::strategy::strategy &
   strategy ();

   /*!
     \brief This is our request queue where pending Paxos requests are queued

     Since our paxos protocol only allows one proposal at the same time, this queue ensures
     that this guarantee is provided.
    */

   request_queue::queue <strategy::request> &
   request_queue ();

private:

   processor_type                               processor_;
   detail::strategy::strategy *                 strategy_;
   request_queue::queue <strategy::request>     request_queue_;
};

}; };

#include "paxos_context.inl"

#endif //! LIBPAXOS_CPP_DETAIL_PAXOS_CONTEXT_HPP
