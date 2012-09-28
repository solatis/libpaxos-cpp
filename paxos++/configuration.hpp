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
    */
   static uint32_t heartbeat_interval;

   /*!
     \brief Timeout (in milliseconds) before marking a host as dead
    */
   static uint32_t timeout;
};

}

#endif  //! LIBPAXOS_CPP_SERVER_HPP
