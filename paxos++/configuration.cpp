#include "durable/storage.hpp"
#include "durable/heap.hpp"
#include "detail/strategy/basic_paxos/factory.hpp"
#include "configuration.hpp"

#include "detail/util/debug.hpp"

namespace paxos {

configuration::configuration ()
   : timeout_ (3000),
     majority_factor_ (0.5),
     durable_storage_ (new durable::heap ()),
     strategy_factory_ (new detail::strategy::basic_paxos::factory (*this))
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
configuration::set_majority_factor (
   double    factor)
{
   majority_factor_ = factor;
}

double
configuration::majority_factor () const
{
   return majority_factor_;
}

void
configuration::set_strategy_factory (
   detail::strategy::factory *  factory)
{
   strategy_factory_.reset (factory);
}

detail::strategy::factory const &
configuration::strategy_factory () const
{
   return *strategy_factory_;
}


void
configuration::set_durable_storage (
   durable::storage *   storage)
{
   durable_storage_.reset (storage);
}

durable::storage &
configuration::durable_storage ()
{
   return *durable_storage_;
}

};
