#include <vector>

#include <boost/ref.hpp>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>

#include "../../quorum.hpp"
#include "../debug.hpp"
#include "../connection_pool.hpp"
#include "protocol.hpp"

namespace paxos { namespace detail { namespace protocol {

protocol::protocol (
   paxos::detail::connection_pool &     connection_pool,
   paxos::quorum &                      quorum)
   : connection_pool_ (connection_pool),
     quorum_ (quorum),

     elect_leader_ (*this)
{
}

void
protocol::bootstrap ()
{
   PAXOS_ASSERT (elect_leader_.size () == 0);
   
   elect_leader_.create ().start ();
}


void
protocol::new_connection (
   tcp_connection &     connection)
{
   PAXOS_DEBUG ("received new connection!");
}

paxos::detail::connection_pool &
protocol::connection_pool ()
{
   return connection_pool_;
}

paxos::quorum &
protocol::quorum ()
{
   return quorum_;
}

}; }; };
