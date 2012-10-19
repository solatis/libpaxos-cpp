#include "detail/strategy/basic_paxos/factory.hpp"
#include "configuration.hpp"

namespace paxos {

configuration::configuration ()
   : timeout_ (3000),
     strategy_factory_ (new detail::strategy::basic_paxos::factory ())
{
}


void
configuration::set_timeout (
   uint32_t  timeout)
{
   timeout_ = timeout;
}

uint32_t
configuration::timeout () const
{
   return timeout_;
}


void
configuration::set_strategy_factory (
   detail::strategy::factory *  factory)
{
   strategy_factory_ .reset (factory);
}


detail::strategy::factory const &
configuration::strategy_factory () const
{
   return *strategy_factory_;
}


};
