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
  \brief Our queue with pending Paxos requests as received by leader

  When a request is being processed, this request is put on this queue and being processed.
  The guard then uses RAII to "lock" this queue while a request is being processed, and
  releases this lock & checks for additional requests on the queue when it goes our of scope.

  It is therefore important that all paxos implementations acquire such a lock.
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
