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
  \brief Provides base class for durable storage components, which acts as the collective 
         memory of the quorum
 */
class storage
{
public:

   storage ();

   /*!
     \brief Destructor
    */
   virtual ~storage ();


   /*!
     \brief Controls the amount of proposed values that should be stored in the durable::storage
     \param amount The minimal amount of values to store

     This controls the amount of proposed values that should at least be stored in the 
     durable::storage. Periodically, libpaxos-cpp issues a cleanup command to the durable::storage
     component to remove old proposed values that aren't used anymore. This variable controls the
     greediness of this cleanup mechanism.

     Note that, if a follower in the paxos quorum is temporarily unreachable, a cleanup will never
     be issued for the values proposed while the follower is unreachable. The storage component 
     never cleans logs if we have less than this amount of logs in history.

     Defaults to 10000.
   */
   void
   set_history_size (
      int64_t   amount);

   /*!
     \brief Access to the amount of values to keep in storage
    */
   int64_t
   history_size () const;
   

   /*!
     \brief Accepts a new value
     \param proposal_id         Proposal id of value
     \param byte_array          The actual value
     \param lowest_proposal_id  Highest proposal_id that has been accepted by the entire quorum

     This function calls store (), and if the size of the history is growing too large
     calls for a cleanup.
    */
   void
   accept (
      int64_t                   proposal_id,
      std::string const &       byte_array,
      int64_t                   lowest_proposal_id);

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

   /*!
     \brief Looks up the lowest proposal id currently stored
     \returns Returns the lowest proposal id in history, or 0 if no previous proposals are stored

     This functionality is not really required by the paxos library, but is used by the test cases.
    */
   virtual int64_t
   lowest_proposal_id () = 0;

protected:

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
     \brief Remove history for proposals with an id lower than \c proposal_id
     \param proposal_id The proposal_id to check for
    */
   virtual void
   remove (
      int64_t                   proposal_id) = 0;

private:

   int64_t      history_size_;

};

} }

#endif  //! LIBPAXOS_CPP_DURABLE_STORAGE_HPP
