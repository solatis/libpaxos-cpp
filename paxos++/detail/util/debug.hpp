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

#ifdef DEBUG

/*!
  Include the proper logging facility.
 */
#include "debug_log_log4cxx.hpp"

#else //!

#define PAXOS_DEBUG(msg)
#define PAXOS_INFO(msg) 
#define PAXOS_WARN(msg) 
#define PAXOS_ERROR(msg)
#define PAXOS_FATAL(msg)

#endif //!

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
   if((check) == false) {                                         \
      PAXOS_FATAL (BOOST_PP_STRINGIZE(check) << " == false");     \
                                                                  \
      abort ();                                                   \
   };

#define PAXOS_ASSERT_EQ(lhs, rhs)                                  \
   if ((lhs == rhs) == false) {                                    \
      PAXOS_FATAL (BOOST_PP_STRINGIZE (lhs) << " != " << BOOST_PP_STRINGIZE (rhs) << " [" << lhs << " != " << rhs << "]"); \
      abort ();                                                         \
   };


#define PAXOS_ASSERT_NE(lhs, rhs)                                  \
   if ((lhs != rhs) == false) {                                   \
      PAXOS_FATAL (BOOST_PP_STRINGIZE (lhs) << " == " << BOOST_PP_STRINGIZE (rhs) << " [" << lhs << " == " << rhs << "]"); \
      abort ();                                                         \
   };


#define PAXOS_ASSERT_GT(lhs, rhs)                                  \
   if ((lhs > rhs) == false) {                                   \
      PAXOS_FATAL (BOOST_PP_STRINGIZE (lhs) << " <= " << BOOST_PP_STRINGIZE (rhs) << " [" << lhs << " <= " << rhs << "]"); \
      abort ();                                                         \
   };

#define PAXOS_ASSERT_GE(lhs, rhs)                                  \
   if ((lhs >= rhs) == false) {                                   \
      PAXOS_FATAL (BOOST_PP_STRINGIZE (lhs) << " < " << BOOST_PP_STRINGIZE (rhs) << " [" << lhs << " < " << rhs << "]"); \
      abort ();                                                         \
   };


#define PAXOS_ASSERT_LT(lhs, rhs)                                  \
   if ((lhs < rhs) == false) {                                   \
      PAXOS_FATAL (BOOST_PP_STRINGIZE (lhs) << " >= " << BOOST_PP_STRINGIZE (rhs) << " [" << lhs << " >= " << rhs << "]"); \
      abort ();                                                         \
   };

#define PAXOS_ASSERT_LE(lhs, rhs)                                  \
   if ((lhs <= rhs) == false) {                                        \
      PAXOS_FATAL (BOOST_PP_STRINGIZE (lhs) << " > " << BOOST_PP_STRINGIZE (rhs) << " [" << lhs << " > " << rhs << "]"); \
      abort ();                                                         \
   };

#define PAXOS_ASSERT_THROW(check, exception) \
   try \
   { \
      check; \
      PAXOS_FATAL (BOOST_PP_STRINGIZE (check) << " did not throw: " << BOOST_PP_STRINGIZE (exception));\
      abort ();\
   } catch (exception) {\
   }

/*!
  Hides the 'missing return value on function returning non-void' warnings; note that
  this aborts when it is actually reached.
 */
#define PAXOS_UNREACHABLE()     __builtin_unreachable()

#endif  //! LIBPAXOS_CPP_PAXOS_DETAIL_DEBUG_HPP
