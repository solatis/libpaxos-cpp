/*!
  Copyright (c) 2012, Leon Mergen, all rights reserved.
 */

#ifndef LIBPAXOS_CPP_DETAIL_PAXOS_CONTEXT_HPP
#define LIBPAXOS_CPP_DETAIL_PAXOS_CONTEXT_HPP

#include <stdint.h>
#include <string>

#include <boost/function.hpp>

#include "paxos_request_queue.hpp"

namespace paxos { namespace detail { namespace strategy {
class factory;
class strategy;
}; }; };


namespace paxos { namespace detail {

/*!
  \brief Keeps track of context information required by the various Paxos protocol implementations
 */
class paxos_context : private boost::noncopyable
{
public:

   typedef boost::function <std::string (std::string const &)>  processor_type;

public:

   paxos_context (
      processor_type const &            processor,
      detail::strategy::factory *       strategy_factory);

   ~paxos_context ();

   uint64_t &
   proposal_id ();

   uint64_t
   proposal_id () const;

   processor_type const &
   processor () const;

   detail::strategy::strategy const &
   strategy () const;

   /*!
     \brief This is our request queue where pending Paxos requests are queued

     Since our paxos protocol only allows one proposal at the same time, this queue ensures
     that this guarantee is provided.
    */

   paxos_request_queue &
   request_queue ();

private:

   uint64_t                     proposal_id_;

   processor_type               processor_;
   detail::strategy::strategy * strategy_;
   paxos_request_queue          request_queue_;
};

}; };

#include "paxos_context.inl"

#endif //! LIBPAXOS_CPP_DETAIL_PAXOS_CONTEXT_HPP
