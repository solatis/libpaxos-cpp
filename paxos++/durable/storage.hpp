/*!
  Copyright (c) 2012, Leon Mergen, all rights reserved.
 */

#ifndef LIBPAXOS_CPP_DURABLE_STORAGE_HPP
#define LIBPAXOS_CPP_DURABLE_STORAGE_HPP

#include <stdint.h>

#include <map>
#include <string>

#include <boost/function.hpp>

namespace paxos { namespace durable {

/*!
  \brief Provides base class for durable storage components
 */
class storage
{
public:

   /*!
     \brief Destructor
    */
   virtual ~storage ();

   /*!
     \brief Stores an accepted value
    */
   virtual void
   store (
      int64_t                   proposal_id,
      std::string const &       byte_array) = 0;

   /*!
     \brief Looks up all recently accepted values starting with \c proposal_id
    */
   virtual std::map <int64_t, std::string>
   retrieve (
      int64_t                   proposal_id) = 0;

   /*!
     \brief Looks up the highest proposal id currently stored
    */
   virtual int64_t
   highest_proposal_id () = 0;

private:

};

} }

#endif  //! LIBPAXOS_CPP_DURABLE_STORAGE_HPP
