#include "heap.hpp"

namespace paxos { namespace durable {


/*! virtual */ void
heap::store (
   int64_t                   proposal_id,
   std::string const &       byte_array)
{
   data_[proposal_id] = byte_array;
}

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

   return data_.rbegin ()->first;
}


}; };
