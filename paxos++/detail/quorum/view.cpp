#include <functional>
#include <boost/uuid/uuid_io.hpp>

#include "../util/debug.hpp"
#include "view.hpp"

namespace paxos { namespace detail { namespace quorum {


view::view (
   boost::asio::io_service &    io_service)
   : io_service_ (io_service)
{
}


void
view::add (
   boost::asio::ip::tcp::endpoint const &       endpoint)
{
   servers_.insert (std::make_pair (endpoint, 
                                    detail::quorum::server (io_service_,
                                                            endpoint)));
}

detail::quorum::server &
view::lookup_server (
   boost::asio::ip::tcp::endpoint const &       endpoint)
{
   auto pos = servers_.find (endpoint);
   PAXOS_ASSERT (pos != servers_.end ());
   return pos->second;
}

detail::quorum::server const &
view::lookup_server (
   boost::asio::ip::tcp::endpoint const &       endpoint) const
{
   auto pos = servers_.find (endpoint);
   PAXOS_ASSERT (pos != servers_.end ());
   return pos->second;
}

std::vector <boost::asio::ip::tcp::endpoint>
view::live_servers ()
{
   std::vector <boost::asio::ip::tcp::endpoint> result;

   for (auto & i : servers_)
   {
      if (i.second.has_connection () == true)
      {
         result.push_back (i.first);
      }
      else
      {
         i.second.establish_connection ();
      }
   }


   /*!
     Validate that the servers array is sorted
    */
   PAXOS_ASSERT (
      std::adjacent_find (result.begin (), result.end (),
                          std::greater <boost::asio::ip::tcp::endpoint> ()) == result.end ());


   return result;
}

void
view::connection_died (
   boost::asio::ip::tcp::endpoint const &       endpoint)
{
   lookup_server (endpoint).reset_connection ();

   /*!
     It is tempting to reset the uuid of the remote host at this point, but we won't.

     Resetting the uuid of remote servers is only for clarity, and shouldn't really be necessary.
     In case a remote server is rebooted, it would broadcast his new uuid very fast anyway,
     and if he would be our new leader, he would reject any paxos requests.
    */
}

int64_t
view::lowest_proposal_id () const
{
   int64_t lowest_proposal_id = -1;

   for (auto const & i : servers_)
   {
      if (lowest_proposal_id == -1)
      {
         lowest_proposal_id = i.second.highest_proposal_id ();
      }
      else
      {
         lowest_proposal_id = std::min (lowest_proposal_id,
                                        i.second.highest_proposal_id ());
      }
   }

   return lowest_proposal_id;
}

}; }; };
