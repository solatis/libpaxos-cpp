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
  \brief Thrown when the callback function of multiple paxos::server instances  do not reply with the same output for the same input
 */
class inconsistent_response : virtual public exception {};


/*!
  \brief Thrown when there is no known leader in the quorum
 */
class no_leader : virtual public exception {};

/*!
  \brief Thrown when a follower does not agree with the proposal id being sent by a leader

  Usually occurs in leader failover or at the end of a netsplit.
 */
class incorrect_proposal : virtual public exception {};

/*!
  \brief Thrown when a follower/client connection has unexpectedly closed
 */
class connection_close : virtual public exception {};

} };

#endif  //! LIBPAXOS_CPP_EXCEPTION_EXCEPTION_HPP
