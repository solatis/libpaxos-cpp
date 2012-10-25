/*!
  Copyright (c) 2012, Leon Mergen, all rights reserved.
 */

#ifndef LIBPAXOS_CPP_EXCEPTION_EXCEPTION_HPP
#define LIBPAXOS_CPP_EXCEPTION_EXCEPTION_HPP

#include <exception>
#include <boost/exception/exception.hpp>

namespace paxos { namespace exception {

/*!
  \brief Base class for all our exceptions
 */
class exception : virtual public std::exception,
                  virtual public boost::exception {};

/*!
  \brief Thrown when the callback function of multiple paxos::server instances 
         do not reply with the same output for the same input
  
  As described in the documentation of paxos::server, it is a requirement of the 
  callback function of a paxos::server to always return the exact same response for
  similar commands. 

  In other words, consider this example:

  \li server 1 replies to paxos message 123 with response "foo"
  \li server 2 replies to paxos message 123 with response "bar"
  \li since the responses do not match, an exception will be thrown.

  
 */
class inconsistent_response : virtual public exception {};


/*!
  \brief Thrown when there is no known leader in the quorum

  Usually occurs right after launching a quorum and should recover upon next retry.
 */
class no_leader : virtual public exception {};


/*!
  \brief Thrown when there is no majority of servers reachable to the current leader

  There is a built-in protection in the library to ensure that at least a certain percentage
  of followers are reachable in order to make progress. This prevents the situation that multiple
  representations of the current truth exist within the same quorum of servers.

  There are two likely situations that this exception can be thrown:

  \li not enough servers are alive;
  \li enough servers are alive, but due to networking error (netsplits) they are not all reachable
      to each other.

  The second scenario is the situation this exception helps the most in: in case of one datacenter
  being cut off from the outside world, it prevents that datacenter of building up a different
  understanding of the truth compared to the other, alive, datacenters.
 */
class no_majority : virtual public exception {};


/*!
  \brief Thrown when a follower does not agree with the proposal id being sent by a leader

  Usually occurs in leader failover or at the end of a netsplit and should recover upon next retry.
 */
class incorrect_proposal : virtual public exception {};

/*!
  \brief Thrown when a follower/client connection has unexpectedly closed

  Usually occurs after a follower has disconnected from the quorum and should recover upon next retry.
 */
class connection_close : virtual public exception {};

/*!
  \brief Thrown when an error occured with a durable storage component
 */
class storage_error : virtual public exception {};

} };

#endif  //! LIBPAXOS_CPP_EXCEPTION_EXCEPTION_HPP
