/*!
  Copyright (c) 2012, Leon Mergen, all rights reserved.
 */

#ifndef LIBPAXOS_CPP_PAXOS_DETAIL_DEBUG_LOG_LOG4CXX_HPP
#define LIBPAXOS_CPP_PAXOS_DETAIL_DEBUG_LOG_LOG4CXX_HPP

#include <log4cxx/logger.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/consoleappender.h>
#include <log4cxx/patternlayout.h>

#include "singleton.hpp"

namespace paxos { namespace detail { namespace util {

/*!
  \brief Helper class that defines a single log4cxx interface
 */
class log4cxx_helper
{

public:

   /*!
     \brief Default constructor initializes the log context
    */
   log4cxx_helper ();

   /*!
     \brief Access to the underlying logger_ instance
    */
   log4cxx::LoggerPtr
   logger ();

private:

   log4cxx::LoggerPtr   logger_;

};


}; }; };


#include "debug_log_log4cxx.inl"

typedef paxos::detail::util::singleton <paxos::detail::util::log4cxx_helper> log4cxx_singleton;

#define PAXOS_DEBUG(msg) LOG4CXX_DEBUG (log4cxx_singleton::instance ().logger (), msg)
#define PAXOS_INFO(msg)  LOG4CXX_INFO  (log4cxx_singleton::instance ().logger (), msg)
#define PAXOS_WARN(msg)  LOG4CXX_WARN  (log4cxx_singleton::instance ().logger (), msg)
#define PAXOS_ERROR(msg) LOG4CXX_ERROR (log4cxx_singleton::instance ().logger (), msg)
#define PAXOS_FATAL(msg) LOG4CXX_FATAL (log4cxx_singleton::instance ().logger (), msg)

#endif //! LIBPAXOS_CPP_PAXOS_DETAIL_DEBUG_LOG_LOG4CXX_HPP
