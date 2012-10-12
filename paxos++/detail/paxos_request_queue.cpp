#include "util/debug.hpp"

#include "strategy/strategy.hpp"
#include "paxos_context.hpp"
#include "paxos_request_queue.hpp"


namespace paxos { namespace detail {


paxos_request_queue::guard::guard (
   paxos_request_queue &        queue)
   : queue_ (queue)
{
   PAXOS_ASSERT (queue.request_being_processed () == false);
   queue_.request_being_processed () = true;
}

paxos_request_queue::guard::~guard ()
{
   PAXOS_ASSERT (queue_.queue_.empty () == false);
   PAXOS_ASSERT (queue_.request_being_processed () == true);

   queue_.queue_.pop ();
   queue_.request_being_processed () = false;

   if (queue_.queue_.empty () == false)
   {
      /*!
        This means we still have requests waiting in line
       */
      paxos_request & request = queue_.queue_.front ();

      request.global_state_.strategy ().initiate (request.client_connection_,
                                                  request.command_,
                                                  request.quorum_,
                                                  request.global_state_,
                                                  guard::create (queue_));
   }
}

/*! static */ paxos_request_queue::guard::pointer
paxos_request_queue::guard::create (
   paxos_request_queue &        queue)
{
   return pointer (new guard (queue));
}


void
paxos_request_queue::push (
   paxos_request &&     request)
{
   queue_.push (request);

   if (request_being_processed () == false)
   {
      request.global_state_.strategy ().initiate (request.client_connection_,
                                                  request.command_,
                                                  request.quorum_,
                                                  request.global_state_,
                                                  guard::create (*this));
   }
}


}; };
