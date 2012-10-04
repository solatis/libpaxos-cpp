/*!
  Copyright (c) 2012, Leon Mergen, all rights reserved.
 */

#ifndef LIBPAXOS_CPP_DETAIL_PAXOS_STATE_HPP
#define LIBPAXOS_CPP_DETAIL_PAXOS_STATE_HPP

#include <stdint.h>
#include <string>

#include <boost/function.hpp>

namespace paxos { namespace detail {

/*!
  \brief Keeps track of state information required by the various Paxos protocol implementations
 */
class paxos_state : private boost::noncopyable
{
public:

   typedef boost::function <std::string (std::string const &)>  processor_type;

public:

   paxos_state (
      processor_type const &    processor);

   uint64_t &
   proposal_id ();

   uint64_t
   proposal_id () const;


   processor_type const &
   processor () const;

private:

   uint64_t             proposal_id_;

   processor_type       processor_;

};

}; };

#include "paxos_state.inl"

#endif //! LIBPAXOS_CPP_DETAIL_PAXOS_STATE_HPP
