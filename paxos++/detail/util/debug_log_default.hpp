/*!
  Copyright (c) 2012, Leon Mergen, all rights reserved.
 */

#ifndef LIBPAXOS_CPP_PAXOS_DETAIL_DEBUG_LOG_DEFAULT_HPP
#define LIBPAXOS_CPP_PAXOS_DETAIL_DEBUG_LOG_DEFAULT_HPP

#define PAXOS_LOG(level, msg) \
   do {                                                    \
      std::clog << level << " [" << __FILE__ << ":" << __LINE__ << "] " << msg << std::endl; \
   } while (false);

#define PAXOS_DEBUG(msg) PAXOS_LOG ("DEBUG", msg)
#define PAXOS_INFO(msg)  PAXOS_LOG ("INFO", msg)
#define PAXOS_WARN(msg)  PAXOS_LOG ("WARN", msg)
#define PAXOS_ERROR(msg) PAXOS_LOG ("ERROR", msg)
#define PAXOS_FATAL(msg) PAXOS_LOG ("FATAL", msg)

#endif //! LIBPAXOS_CPP_PAXOS_DETAIL_DEBUG_LOG_DEFAULT_HPP
