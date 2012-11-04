#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "../util/debug.hpp"
#include "../../configuration.hpp"
#include "server_view.hpp"

namespace paxos { namespace detail { namespace quorum {


server_view::server_view (
   boost::asio::io_service &                    io_service,
   boost::asio::ip::tcp::endpoint const &       endpoint,
   paxos::configuration const &                 configuration)
   : view::view (io_service),
     majority_factor_ (configuration.majority_factor ()),
     our_endpoint_ (endpoint)
{
   this->add (endpoint);

   /*!
     Since this is the constructor for a server, we need to initialize our own
     entry inside the quorum:

     * to initialize a UUID for our host, which we will communicate to other hosts inside
       the quorum and use to decide who should be the leader of the quorum;

    */
   boost::uuids::basic_random_generator <boost::mt19937> gen;

   lookup_server (endpoint).set_id (gen ());
}

boost::asio::ip::tcp::endpoint const &
server_view::our_endpoint () const
{
   return our_endpoint_;
}

bool
server_view::has_majority ()
{
   PAXOS_DEBUG ("has_majority live_servers.size () = " << this->live_servers ().size () << ", servers_.size () = " << servers_.size ());

   return
      static_cast <double> (this->live_servers ().size ()) 
      >= (static_cast <double> (servers_.size ()) * majority_factor_);
}

boost::optional <boost::asio::ip::tcp::endpoint>
server_view::who_is_our_leader ()
{
   /*!
     We can only possibly select a leader from all live servers.
    */
   std::vector <boost::asio::ip::tcp::endpoint> live_servers = this->live_servers ();

   if (live_servers.empty () == true)
   {
      return boost::none;
   }

   int64_t highest_proposal_id = -1;

   std::map <boost::uuids::uuid, boost::asio::ip::tcp::endpoint> sorted;
   for (auto const & endpoint : live_servers)
   {
      detail::quorum::server & server = lookup_server (endpoint);

      if (server.has_id () == true)
      {
         sorted[server.id ()] = endpoint;
         highest_proposal_id  = std::max (highest_proposal_id,
                                          server.highest_proposal_id ());
      }
   }

   if (sorted.empty () == true)
   {
      /*!
        This means we only have live hosts with unknown ids.
       */
      return boost::none;
   }

   /*!
     Our leader election algorithm selects the node with the *lowest* uuid as our
     leader. This has a very important reason: when we do not have a uuid for a host yet,
     this host will always be selected as a leader. Since uuid handshakes take place in
     each request, it will ensure we will get the correct uuid within the next request.

     Otherwise, the host selected is the one with the lowest 'real' uuid, which is a fine
     leader selection algorithm.
    */
   for (auto const & i : sorted)
   {
      /*!
        This ensures that we do not select a host that is lagging behind as our leader. Don't
        worry, he will catch up!
       */

      PAXOS_DEBUG ("server " << i.second << " has proposal_id = " << lookup_server (i.second).highest_proposal_id () << ", highest_proposal_id = " << highest_proposal_id);

      if (lookup_server (i.second).highest_proposal_id () == highest_proposal_id)
      {
         return i.second;
      }
   }

   PAXOS_UNREACHABLE ();
}


}; }; };
