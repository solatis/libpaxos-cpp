/*!
  Copyright (c) 2012, Leon Mergen, all rights reserved.
 */

#ifndef LIBPAXOS_CPP_CONFIGURATION_HPP
#define LIBPAXOS_CPP_CONFIGURATION_HPP

#include <stdint.h>

#include <boost/shared_ptr.hpp>

namespace paxos { namespace detail { namespace strategy {
class factory;
}; }; };

namespace paxos {

/*!
  \brief Runtime configurable parameters
 */
class configuration
{

public:

   /*!
     \brief Default constructor
    */
   configuration ();

   /*!
     \brief Adjusts interval (in milliseconds) between heartbeats

     Default is 3000 (3 seconds)
    */
   void
   set_heartbeat_interval (
      uint32_t  heartbeat_interval);

   /*!
     \brief Access to heartbeat interval
    */
   uint32_t
   heartbeat_interval () const;


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
     \brief Adjusts the amount of servers that need to be alive in order for the algorithm to make progress

     Say, for example, you have a quorum of 3 servers, and 1 server is down, this factor would need to be
     lower than 0.667 in order for the algorithm to make progress.

     Defaults is 0.5 (at least 50% of the servers need to be online)
    */
   void
   set_quorum_majority_factor (
      float     quorum_majority_factor);

   float
   quorum_majority_factor () const;

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

private:

   uint32_t                                             heartbeat_interval_;
   uint32_t                                             timeout_;
   float                                                quorum_majority_factor_;

   boost::shared_ptr <detail::strategy::factory>        strategy_factory_;
};

}

#endif  //! LIBPAXOS_CPP_SERVER_HPP
