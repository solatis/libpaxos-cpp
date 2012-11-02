#include "storage.hpp"

namespace paxos { namespace durable {

storage::storage ()
   : history_size_ (10000)
{
}

/*! virtual */ storage::~storage ()
{
}

void
storage::set_history_size (
   int64_t      history_size)
{
   history_size_ = history_size;
}

int64_t
storage::history_size () const
{
   return history_size_;
}

void
storage::accept (
   int64_t              proposal_id,
   std::string const &  byte_array)
{
   store (proposal_id,
          byte_array);

   /*!
     We do not want to issue a cleanup () after every call to accept (), since that
     would be very inefficient. Instead, since we know for sure that proposal_id only
     increments by 1 every call, we just issue a cleanup () call every N calls to accept,
     where N = history_size_.

     This ensures that the size of the history is always between history_size_ and 
     2 * history_size_
    */
   if (proposal_id % history_size_ == 0 

       /*! 
         This check is here to prevent the proposal_id from going negative; should still
         work otherwise, but it's just a bit more pretty this way.
       */
       && proposal_id > history_size_)
   {
      remove (proposal_id - history_size_);
   }
}

}; };
