#include "quorum.hpp"

namespace paxos { 


void
quorum::we_are (
   boost::asio::ip::address const &     address,
   uint16_t                             port,
   boost::uuids::uuid const &           uuid)
{
   uuid_ = uuid;
   self_ = boost::asio::ip::tcp::endpoint (address, port);

   assert (servers_.find (self_) != servers_.end ());
}

boost::asio::ip::tcp::endpoint const &
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

};
