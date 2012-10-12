/*!
  Copyright (c) 2012, Leon Mergen, all rights reserved.
 */

#ifndef LIBPAXOS_CPP_CONFIGURATION_HPP
#define LIBPAXOS_CPP_CONFIGURATION_HPP

#include <stdint.h>

namespace paxos {

/*!
  \brief Runtime configurable parameters
 */
class configuration
{
public:

   /*!
     \brief Interval (in milliseconds) between heartbeats

     Default is 3000 (3 seconds)
    */
   static uint32_t heartbeat_interval;

   /*!
     \brief Timeout (in milliseconds) before marking a host as dead

     Default is 3000 (3 seconds)
    */
   static uint32_t timeout;

   /*!
     \brief Determines the amount of servers that need to be alive in order for the algorithm to make progress

     Say, for example, you have a quorum of 3 servers, and 1 server is down, this factor would need to be
     lower than 0.667 in order for the algorithm to make progress.

     Defaults is 0.5 (at least 50% of the servers need to be online)
    */
   static float quorum_majority_factor;
};

}

#endif  //! LIBPAXOS_CPP_SERVER_HPP
