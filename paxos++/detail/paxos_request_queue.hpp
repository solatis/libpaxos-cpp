/*!
  Copyright (c) 2012, Leon Mergen, all rights reserved.
 */

#ifndef LIBPAXOS_CPP_DETAIL_PAXOS_REQUEST_QUEUE_HPP
#define LIBPAXOS_CPP_DETAIL_PAXOS_REQUEST_QUEUE_HPP

#include <queue>

#include <boost/shared_ptr.hpp>

#include "paxos_request.hpp"

namespace paxos { namespace detail {

/*!
  \brief Our queue with pending Paxos requests as received by leader

  When a request is being processed, this request is put on this queue and being processed.
  The guard then uses RAII to "lock" this queue while a request is being processed, and
  releases this lock & checks for additional requests on the queue when it goes our of scope.

  It is therefore important that all paxos implementations acquire such a lock.
 */
class paxos_request_queue
{

public:

   class guard
   {
   public:
      typedef boost::shared_ptr <guard> pointer;


      ~guard ();

      static pointer
      create (
         paxos_request_queue &  queue);

   private:
      guard (
         paxos_request_queue &  queue);

   private:

      paxos_request_queue &     queue_;
   };

public:

   paxos_request_queue ();

   void
   push (
      paxos_request &&  request);

   bool &
   request_being_processed ();


private:


   bool                         request_being_processed_;
   std::queue <paxos_request>   queue_;
   
};

}; };

#include "paxos_request_queue.inl"

#endif //! LIBPAXOS_CPP_DETAIL_PAXOS_REQUEST_QUEUE_HPP
