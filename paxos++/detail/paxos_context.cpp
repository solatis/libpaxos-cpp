#include "strategy/factory.hpp"
#include "strategy/strategy.hpp"

#include "paxos_context.hpp"

namespace paxos { namespace detail {


paxos_context::paxos_context (
   processor_type const &       processor,
   detail::strategy::factory *  strategy_factory)
   : proposal_id_ (0),
     processor_ (processor),
     strategy_ (strategy_factory->create ())
{
}

paxos_context::~paxos_context ()
{
   delete strategy_;
}


}; };
