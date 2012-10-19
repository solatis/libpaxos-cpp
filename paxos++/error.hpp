/*!
  Copyright (c) 2012, Leon Mergen, all rights reserved.
 */

#ifndef LIBPAXOS_CPP_ERROR_HPP
#define LIBPAXOS_CPP_ERROR_HPP

#include <string>

namespace paxos { 

enum error_code
{
   /*!
     This error is sent back when a specific node receives a request for a leader and is 
     not a leader (or there is no leader yet).
    */
   error_no_leader,

   /*!
     This error is sent back when not all nodes in the quorum can accept a request
     due to incorrect proposal id's. This happends when multiple nodes in the quorum
     think they are a leader, and usually resolves itself after the next heartbeat.
    */
   error_incorrect_proposal,

   /*!
     This error is sent back to the clients when not all nodes in the quorum return the same
     reply to a similar request.
    */
   error_inconsistent_response,

   /*!
     This error is sent back when a connection unexpectedly closed.
    */
   error_connection_close,

   /*!
     This error is sent back when a timeout occured on one of the followers.
    */
   error_connection_timeout
};


std::string
to_string (
   enum error_code);

};

#endif //! LIBPAXOS_CPP_QUORUM_HPP
