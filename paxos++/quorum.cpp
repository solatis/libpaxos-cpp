#include "quorum.hpp"

namespace paxos {

void
quorum::add (
   boost::asio::ip::address const &     address,
   uint16_t                             port)
{
   quorum_.add (address, port);
}

};
