#include "quorum.hpp"

namespace paxos { namespace detail {


void
quorum::add (
   boost::asio::ip::tcp::endpoint const &       endpoint)
{
   servers_.insert (endpoint);
}


}; };
