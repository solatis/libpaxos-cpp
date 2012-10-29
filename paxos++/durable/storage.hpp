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
     \param proposal_id The id of the proposal to store
     \param byte_array  The value that is associated with the proposal

     \par Preconditions
     
     highest_proposal_id () == (proposal_id - 1)

     \par Postconditions

     highest_proposal_id () == proposal_id

    */
   virtual void
   store (
      int64_t                   proposal_id,
      std::string const &       byte_array) = 0;

   /*!
     \brief Looks up all recently accepted values higher than with \c proposal_id

     This function does not necessarily need to return all values. In fact, it is preferred if
     a large catch-up is retrieved in small batches, so that a catch-up can occur gradually 
     instead of in a single paxos round.
   */
   virtual std::map <int64_t, std::string>
   retrieve (
      int64_t                   proposal_id) = 0;

   /*!
     \brief Looks up the highest proposal id currently stored
     \returns Returns highest proposal id in history, or 0 if no previous proposals are stored
    */
   virtual int64_t
   highest_proposal_id () = 0;

private:

};

} }

#endif  //! LIBPAXOS_CPP_DURABLE_STORAGE_HPP
