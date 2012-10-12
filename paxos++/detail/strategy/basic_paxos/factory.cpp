#include "protocol/strategy.hpp"
#include "factory.hpp"

namespace paxos { namespace detail { namespace strategy { namespace basic_paxos {


/*! virtual */ strategy *
factory::create () const
{
   return new protocol::strategy ();
}

}; }; }; };
