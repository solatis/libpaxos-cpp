/*!
  Copyright (c) 2012, Leon Mergen, all rights reserved.
 */

#ifndef LIBPAXOS_CPP_DETAIL_PAXOS_REQUEST_QUEUE_HPP
#define LIBPAXOS_CPP_DETAIL_PAXOS_REQUEST_QUEUE_HPP

#include <boost/function.hpp>

#include <queue>

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

   typedef boost::function <void (Type, typename guard::pointer)>       callback;

public:

   queue (
      callback  callback);

   void
   push (
      
      Type &&  request);

   bool &
   request_being_processed ();


private:

   callback             callback_;
   bool                 request_being_processed_;
   std::queue <Type>    queue_;
   
};

}; }; };

#include "queue.inl"

#endif //! LIBPAXOS_CPP_DETAIL_PAXOS_REQUEST_QUEUE_HPP
