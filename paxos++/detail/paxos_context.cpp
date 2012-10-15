#include "strategy/factory.hpp"
#include "strategy/strategy.hpp"

#include "../configuration.hpp"
#include "paxos_context.hpp"

namespace paxos { namespace detail {


paxos_context::paxos_context (
   processor_type const &       processor,
   paxos::configuration const & configuration)
   : proposal_id_ (0),
     processor_ (processor),
     strategy_ (configuration.strategy_factory ().create ())
{
}

paxos_context::~paxos_context ()
{
   delete strategy_;
}


}; };
