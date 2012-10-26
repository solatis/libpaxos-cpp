/*!
  Copyright (c) 2012, Leon Mergen, all rights reserved.
 */

#ifndef LIBPAXOS_CPP_DETAIL_STRATEGY_STRATEGY_HPP
#define LIBPAXOS_CPP_DETAIL_STRATEGY_STRATEGY_HPP

#include "../tcp_connection_fwd.hpp"
#include "../request_queue/queue.hpp"

#include "request.hpp"

namespace paxos { namespace detail { namespace quorum { 
class server_view;
}; }; };

namespace paxos { namespace detail { 
class command;
class paxos_context;
}; };

namespace paxos { namespace detail { namespace strategy {

/*!
  \brief Abstract base class for a paxos strategy
 */
class strategy
{
protected:

   typedef detail::request_queue::queue <struct request>::guard::pointer queue_guard_type;

public:

   virtual ~strategy ();

   /*!
     \brief Received by leader from client that initiates a request
    */
   virtual void
   initiate (      
      tcp_connection_ptr                client_connection,
      detail::command const &           command,
      detail::quorum::server_view &     quorum,
      detail::paxos_context &           global_state,
      queue_guard_type                  queue_guard) = 0;


   /*!
     \brief Received by follower when leader wants to prepare a request
    */
   virtual void
   prepare (      
      tcp_connection_ptr                leader_connection,
      detail::command const &           command,
      detail::quorum::server_view &     quorum,
      detail::paxos_context &           global_state) = 0;


   /*!
     \brief Received by follower when leader wants to process a request

     This request is received by a follower when a leader has received an 'ACK' for a certain
     proposal id from all followers, and is now telling all followers to actually process a 
     command.
    */
   virtual void
   accept (      
      tcp_connection_ptr                leader_connection,
      detail::command const &           command,
      detail::quorum::server_view &     quorum,
      detail::paxos_context &           global_state) = 0;

private:

};

} }; };

#include "strategy.inl"

#endif  //! LIBPAXOS_CPP_DETAIL_STRATEGY_STRATEGY_HPP
