/*!
  Copyright (c) 2012, Leon Mergen, all rights reserved.
 */

#include <iostream>

#include <boost/thread/mutex.hpp>

#include <boost/throw_exception.hpp>
#include <boost/exception/info_tuple.hpp>
#include <boost/exception/error_info.hpp>
#include <boost/preprocessor/stringize.hpp>

#ifndef LIBPAXOS_CPP_PAXOS_DETAIL_DEBUG_HPP
#define LIBPAXOS_CPP_PAXOS_DETAIL_DEBUG_HPP

static boost::mutex paxos_log_mutex;

#ifdef DEBUG

#define PAXOS_LOG(level, msg) \
   { \
     boost::mutex::scoped_lock lock (paxos_log_mutex);\
     std::cerr << level << " [" << __FILE__ << ":" << __LINE__ << "] " << msg << std::endl; \
     std::cerr.flush ();                                                \
   }
     


#define PAXOS_DEBUG(msg) PAXOS_LOG ("DEBUG", msg)
#define PAXOS_INFO(msg)  PAXOS_LOG ("INFO", msg)
#define PAXOS_WARN(msg)  PAXOS_LOG ("WARN", msg)
#define PAXOS_ERROR(msg) PAXOS_LOG ("ERROR", msg)
#define PAXOS_FATAL(msg) PAXOS_LOG ("FATAL", msg)

#else //! #ifdef DEBUG

#define PAXOS_DEBUG(msg)
#define PAXOS_INFO(msg) 
#define PAXOS_WARN(msg) 
#define PAXOS_ERROR(msg)
#define PAXOS_FATAL(msg)

#endif //! #ifdef DEBUG

namespace paxos {
typedef boost::error_info <struct tag_solatis_debug, std::string> debug_exception_info;
};

#define PAXOS_CHECK_THROW(check,exception)\
   if (check) {                                                   \
      BOOST_THROW_EXCEPTION (exception << paxos::debug_exception_info (#check)); \
   };

#define PAXOS_THROW(exception)\
   BOOST_THROW_EXCEPTION (exception);


#define PAXOS_ASSERT(check)\
   do {                                                           \
      if((check) == false) {                                      \
         PAXOS_FATAL (BOOST_PP_STRINGIZE(check) << " == false");  \
                                                                  \
         abort ();                                                \
      };                                                          \
   } while (false);


/*!
  Hides the 'missing return value on function returning non-void' warnings; note that
  this aborts when it is actually reached.
 */
#define PAXOS_UNREACHABLE()     __builtin_unreachable()



#endif  //! LIBPAXOS_CPP_PAXOS_DETAIL_DEBUG_HPP
