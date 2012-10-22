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
  \brief Thrown when the quorum is in an inconsistent state
 */
class not_ready : virtual public exception {};

/*!
  \brief Thrown when an unexpected command arrives
 */
class protocol_error : virtual public exception {};

/*!
  \brief Thrown when a request could not be completed
 */
class request_error : virtual public exception {};

/*!
  \brief Thrown when the callback function of multiple paxos::server instances  do not reply with the same output for the same input
 */
class inconsistent_response : virtual public exception {};

} };

#endif  //! LIBPAXOS_CPP_EXCEPTION_EXCEPTION_HPP
