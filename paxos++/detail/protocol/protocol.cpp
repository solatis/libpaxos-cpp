#include <vector>

#include <boost/ref.hpp>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/detail/endian.hpp>

#include "../util/debug.hpp"
#include "../util/conversion.hpp"

#include "../../quorum.hpp"
#include "../connection_pool.hpp"
#include "pb/adapter.hpp"
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


/*! static */ void
protocol::write_command (
   pb::command const &          command,
   detail::tcp_connection &     destination)
{
   std::string binary_string = pb::adapter::to_string (command);
   uint32_t size             = binary_string.size ();

   std::string buffer        = util::conversion::to_byte_array (size) + binary_string;

   destination.write (buffer);
}


}; }; };
