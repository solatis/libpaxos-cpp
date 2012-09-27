#include "detail/util/debug.hpp"
#include "quorum.hpp"

namespace paxos { 


void
quorum::we_are (
   boost::asio::ip::address const &     address,
   uint16_t                             port,
   boost::uuids::uuid const &           id,
   enum detail::remote_server::state    state)
{
   self_.endpoint_ = boost::asio::ip::tcp::endpoint (address, port);
   self_.id_       = id;
   self_.state_    = state;

   PAXOS_ASSERT (servers_.find (self_.endpoint_) != servers_.end ());
}

struct quorum::self const &
quorum::self () const
{
   return self_;
}

void
quorum::add (
   boost::asio::ip::address const &     address,
   uint16_t                             port)
{
   detail::remote_server server;
   servers_[boost::asio::ip::tcp::endpoint (address, port)] = server;
}



detail::remote_server &
quorum::lookup (
   boost::asio::ip::tcp::endpoint const &    endpoint)
{
   auto pos = servers_.find (endpoint);

   PAXOS_ASSERT (pos != servers_.end ());

   return pos->second;
}

detail::remote_server const &
quorum::lookup (
   boost::asio::ip::tcp::endpoint const &    endpoint) const
{
   auto pos = servers_.find (endpoint);

   PAXOS_ASSERT (pos != servers_.end ());

   return pos->second;
}



};
