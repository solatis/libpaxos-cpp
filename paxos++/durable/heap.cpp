#include "../detail/util/debug.hpp"

#include "heap.hpp"

namespace paxos { namespace durable {


/*! virtual */ std::map <int64_t, std::string>
heap::retrieve (
   int64_t      proposal_id)
{
   std::map <int64_t, std::string> result;

   result.insert (data_.find (proposal_id + 1), data_.end ());

   return result;
}

/*! virtual */ int64_t
heap::highest_proposal_id ()
{
   if (data_.empty () == true)
   {
      return 0;
   }

   PAXOS_DEBUG (this << " highest_proposal_id = " << data_.rbegin ()->first);

   return data_.rbegin ()->first;
}

/*! virtual */ int64_t
heap::lowest_proposal_id ()
{
   if (data_.empty () == true)
   {
      return 0;
   }

   return data_.begin ()->first;
}


/*! virtual */ void
heap::store (
   int64_t                   proposal_id,
   std::string const &       byte_array)
{
   PAXOS_DEBUG (this << " proposal_id = " << proposal_id << ", highest_proposal_id = " << highest_proposal_id ());

   PAXOS_ASSERT_EQ (
      proposal_id, highest_proposal_id () + 1);

   data_[proposal_id] = byte_array;
}

void
heap::remove (
   int64_t              proposal_id)
{
   PAXOS_ASSERT (proposal_id >= 0);

   if (data_.find (proposal_id) == data_.end ())
   {
      /*!
        This is weird, likely caused by some manual copying / fiddling with the data
        backend. Only sensible thing to do is to ignore this remove command.
       */
      PAXOS_WARN ("proposal_id " << proposal_id << " not found in history, ignoring remove!");
      return;
   }

   PAXOS_DEBUG (this << " deleting all data since " << proposal_id);

   data_.erase (data_.begin (),
                data_.find (proposal_id));
   data_.erase (proposal_id);

   PAXOS_ASSERT_EQ (lowest_proposal_id (), proposal_id + 1);

}

}; };
