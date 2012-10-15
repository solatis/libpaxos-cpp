#include "detail/strategy/basic_paxos/factory.hpp"
#include "configuration.hpp"

namespace paxos {

configuration::configuration ()
   : heartbeat_interval_ (3000),
     timeout_ (3000),
     quorum_majority_factor_ (0.5),
     strategy_factory_ (new detail::strategy::basic_paxos::factory ())
{
}

void
configuration::set_heartbeat_interval (
   uint32_t  heartbeat_interval)
{
   heartbeat_interval_ = heartbeat_interval;
}

uint32_t
configuration::heartbeat_interval () const
{
   return heartbeat_interval_;
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
configuration::set_quorum_majority_factor (
   float     quorum_majority_factor) 
{
   quorum_majority_factor_ = quorum_majority_factor;
}

float
configuration::quorum_majority_factor () const
{
   return quorum_majority_factor_;
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
