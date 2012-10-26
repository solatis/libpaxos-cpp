#include "../../../configuration.hpp"

#include "protocol/strategy.hpp"
#include "factory.hpp"

namespace paxos { namespace detail { namespace strategy { namespace basic_paxos {

factory::factory (
   paxos::configuration &       configuration)
   : configuration_ (configuration)
{
}

/*! virtual */ strategy *
factory::create () const
{
   return new protocol::strategy (configuration_.durable_storage ());
}

}; }; }; };
