/*!
  Copyright (c) 2012, Leon Mergen, all rights reserved.
 */

#ifndef LIBPAXOS_CPP_DETAIL_PAXOS_REQUEST_QUEUE_HPP
#define LIBPAXOS_CPP_DETAIL_PAXOS_REQUEST_QUEUE_HPP

#include <queue>

#include <boost/function.hpp>
#include <boost/optional.hpp>
#include <boost/thread/mutex.hpp>

#include "../util/debug.hpp"

namespace paxos { namespace detail { namespace request_queue {

/*!
  \brief Provides queue with pending requests

  This component is used by in two different places in the paxos library:

  * incoming requests are enqueued here by the leader;
  * outgoing requests are enqueued here by the client.

  The reasons for this is two-fold:

  * the Paxos protocol requires that only one instance of the protocol can be active at
    the same time;
  * Boost.Asio doesn't allow us to do multiple async_write / async_read calls at the same
    time.

  The Boost.Asio problem is one we can overcome, but the Paxos protocol we can't: before a new
  Paxos proposal is sent to the learners, we must first ensure all previous proposals have 
  neded.

  To solve this problem, we provide this queue class. It is a bit of a "magic" queue: new
  requests put on this queue are automatically processed using the callback provided, and
  it ensures a maximum of one request is processed at the same time.

  The magic is in the fact that it provides a so-called "queue guard": the callback automatically
  gets this guard as part of its function parameters, and as soon as this guard goes out of scope,
  a new request is processed.

  Since the thread putting new requests on the queue doesn't necessarily have to be the same
  thread as the one that pulls requests off the queue, this class is thread safe.
 */

template <typename Type>
class queue
{

public:

   class guard
   {
   public:
      typedef boost::shared_ptr <guard> pointer;

      ~guard ();
      
      static pointer
      create (
         queue <Type> &    queue);
   
   private:
      guard (
         queue <Type> &    queue);
      
   private:
      
      queue <Type> &       queue_;
   };

   typedef boost::function <void (Type const &, typename guard::pointer)>       callback;

public:

   queue (
      callback  callback);

   void
   push (
      Type &&  request);

   void
   pop ();

private:

   /*!
     \returns Returns request that callback should be executed on, if any
    */
   boost::optional <Type const &>
   push_locked (
      Type &&  request);

   /*!
     \returns Returns request that callback should be executed on, if any
    */
   boost::optional <Type const &>
   pop_locked ();

   void
   start_request_locked (
      Type &    request);

   

private:

   callback             callback_;

   /*!
     \brief Synchronizes access to request_being_processed_ and queue_

     We need a recursive_mutex since the callback is executed within a locked environment,
     but can in turn generate a push () request within that callback. When that occurs, if
     we would use regular mutexes, a deadlock would occur.
    */
   boost::mutex         mutex_;

   bool                 request_being_processed_;
   std::queue <Type>    queue_;
};

}; }; };

#include "queue.inl"

#endif //! LIBPAXOS_CPP_DETAIL_PAXOS_REQUEST_QUEUE_HPP
