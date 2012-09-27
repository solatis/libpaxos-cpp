/*!
  Copyright (c) 2012, Leon Mergen, all rights reserved.
 */

#include <iostream>

#include <boost/preprocessor/stringize.hpp>

#ifndef LIBPAXOS_CPP_PAXOS_DETAIL_DEBUG_HPP
#define LIBPAXOS_CPP_PAXOS_DETAIL_DEBUG_HPP

#ifdef DEBUG

#define PAXOS_DEBUG(msg) \
   std::cerr << "DEBUG [" << __FILE__ << ":" << __LINE__ << "] " << msg << std::endl;
#define PAXOS_INFO(msg) \
   std::cerr << "INFO  [" << __FILE__ << ":" << __LINE__ << "] " << msg << std::endl;
#define PAXOS_WARN(msg) \
   std::cerr << "WARN  [" << __FILE__ << ":" << __LINE__ << "] " << msg << std::endl;
#define PAXOS_ERROR(msg) \
   std::cerr << "ERROR [" << __FILE__ << ":" << __LINE__ << "] " << msg << std::endl;
#define PAXOS_FATAL(msg) \
   std::cerr << "FATAL [" << __FILE__ << ":" << __LINE__ << "] " << msg << std::endl;

#else //! #ifdef DEBUG

#define PAXOS_DEBUG(msg)
#define PAXOS_INFO(msg) 
#define PAXOS_WARN(msg) 
#define PAXOS_ERROR(msg)
#define PAXOS_FATAL(msg)

#endif //! #ifdef DEBUG


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
