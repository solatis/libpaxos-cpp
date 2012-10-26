/*!
  Copyright (c) 2012, Leon Mergen, all rights reserved.
 */

#ifndef LIBPAXOS_CPP_DURABLE_HEAP_HPP
#define LIBPAXOS_CPP_DURABLE_HEAP_HPP

#include "storage.hpp"

namespace paxos { namespace durable {
/*!
  \brief Provides (non-)durable paxos::server backend based on RAM storage
 */
class heap : public storage
{
public:

public:

   virtual void
   store (
      int64_t                   proposal_id,
      std::string const &       byte_array);

   virtual std::map <int64_t, std::string>
   retrieve (
      int64_t                   proposal_id);

   virtual int64_t
   highest_proposal_id ();

private:

private:

   std::map <int64_t, std::string>      data_;
};

} }

#endif  //! LIBPAXOS_CPP_DURABLE_HEAP_HPP
