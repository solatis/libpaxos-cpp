#include "strategy/factory.hpp"
#include "strategy/strategy.hpp"

#include "../configuration.hpp"
#include "../durable/storage.hpp"
#include "paxos_context.hpp"

namespace paxos { namespace detail {


paxos_context::paxos_context (
   processor_type const &               processor,
   paxos::configuration &               configuration)
   : processor_ (processor),
     strategy_ (configuration.strategy_factory ().create ()),
     request_queue_ (
        []
        (strategy::request const &                                                      request,
         detail::request_queue::queue <detail::strategy::request>::guard::pointer       guard)
        {
           request.global_state_.strategy ().initiate (request.connection_,
                                                       request.command_,
                                                       request.quorum_,
                                                       request.global_state_,
                                                       guard);
        })
{
}

paxos_context::~paxos_context ()
{
   delete strategy_;
}


}; };
