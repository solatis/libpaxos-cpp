/*!
  Copyright (c) 2012, Leon Mergen, all rights reserved.
 */

#ifndef LIBPAXOS_CPP_CONFIGURATION_HPP
#define LIBPAXOS_CPP_CONFIGURATION_HPP

#include <stdint.h>

#include <boost/shared_ptr.hpp>

namespace paxos { namespace durable {
class storage;
}; };

namespace paxos { namespace detail { namespace strategy {
class factory;
}; }; };

namespace paxos {

/*!
  \brief Provides runtime configurable parameters to paxos::client and paxos::server
 */
class configuration
{

public:

   /*!
     \brief Default constructor
    */
   configuration ();

   /*!
     \brief Adjusts timeout (in milliseconds) before marking a host as dead

     Default is 3000 (3 seconds)
    */
   void
   set_timeout (
      uint32_t  timeout);

   /*!
     \brief Access to timeout
    */
   uint32_t
   timeout () const;

   /*!
     \brief Controls the percentage of servers that must be reachable before progress is made
     \param factor The factor of servers that need to be alive
     \pre 0.0 < factor && factor <= 1.0

     Defaults to 0.5
    */
   void
   set_majority_factor (
      double    factor);

   /*!
     \brief Acess to the percentage of servers that must be reachable before progress is made
    */
   double
   majority_factor () const;


   /*!
     \brief Controls the amount of proposed values that should be stored in the durable::storage
     \param amount The minimal amount of values to store

     This controls the amount of proposed values that should at least be stored in the 
     durable::storage. Periodically, libpaxos-cpp issues a cleanup command to the durable::storage
     component to remove old proposed values that aren't used anymore. This variable controls the
     greediness of this cleanup mechanism.

     Note that, if a follower in the paxos quorum is temporarily unreachable, a cleanup will never
     be issued for the values proposed while the follower is unreachable.

     Defaults to 10000.
   */
   void
   set_history_size (
      int64_t   amount);

   /*!
     \brief Adjusts the strategy used for internal paxos protocol
     \note Takes over ownership of \c factory
    */
   void
   set_strategy_factory (
      detail::strategy::factory *       factory);

   /*!
     \brief Access to strategy used for internal paxos protocol
    */
   detail::strategy::factory const &
   strategy_factory () const;

   /*!
     \brief Adjusts the storage component used for durable history
     \note Takes over ownership of \c storage
    */
   void
   set_durable_storage (
      durable::storage *        storage);

   /*!
     \brief Access to the storage component used for a durable history
    */
   durable::storage &
   durable_storage ();

private:

   uint32_t                                             timeout_;
   double                                               majority_factor_;

   boost::shared_ptr <durable::storage>                 durable_storage_;
   boost::shared_ptr <detail::strategy::factory>        strategy_factory_;
};

}

#endif  //! LIBPAXOS_CPP_SERVER_HPP
