/*!
  Copyright (c) 2012, Leon Mergen, all rights reserved.
 */

#ifndef LIBPAXOS_CPP_EXCEPTION_HPP
#define LIBPAXOS_CPP_EXCEPTION_HPP

#include <exception>
#include <boost/exception/exception.hpp>

namespace paxos {

/*!
  \brief Base class for all our exceptions
 */
class exception : virtual public std::exception,
                  virtual public boost::exception {};

/*!
  \brief Thrown when the quorum is in an inconsistent state
 */
class not_ready_exception : virtual public paxos::exception {};

/*!
  \brief Thrown when an unexpected command arrives
 */
class protocol_error_exception : virtual public paxos::exception {};
}

#endif  //! LIBPAXOS_CPP_EXCEPTION_HPP
