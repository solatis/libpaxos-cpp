#include "protocol/strategy.hpp"
#include "factory.hpp"

namespace paxos { namespace detail { namespace strategy { namespace basic_paxos {

factory::factory (
   durable::storage &        storage)
   : storage_ (storage)
{
}

/*! virtual */ strategy *
factory::create () const
{
   return new protocol::strategy (storage_);
}

}; }; }; };
