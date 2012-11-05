/*!
  Copyright (c) 2012, Leon Mergen, all rights reserved.
 */

#ifndef LIBPAXOS_CPP_DURABLE_HEAP_HPP
#define LIBPAXOS_CPP_DURABLE_HEAP_HPP

#include "storage.hpp"

namespace paxos { namespace durable {
/*!
  \brief Provides (non-)durable paxos::server backend based on RAM storage

  This class provides a non-durable storage backend based on RAM usage. It is intended for
  applications that either have a large enough quorum to ensure that at least one paxos::server
  instance will always be available, or do not care about the durability of the operations
  executed on the quorum.

  This is the default storage backend provided by the paxos::configuration class.

  \par Thread Safety
  \e Distinct \e objects: Safe \n
  \e Shared \e objects: Unsafe \n

 */
class heap : public storage
{
public:

public:

   virtual std::map <int64_t, std::string>
   retrieve (
      int64_t                   proposal_id);

   virtual int64_t
   highest_proposal_id ();

   virtual int64_t
   lowest_proposal_id ();

protected:


   virtual void
   store (
      int64_t                   proposal_id,
      std::string const &       byte_array);

   virtual void
   remove (
      int64_t                   proposal_id);


private:

   std::map <int64_t, std::string>      data_;
};

} }

#endif  //! LIBPAXOS_CPP_DURABLE_HEAP_HPP
