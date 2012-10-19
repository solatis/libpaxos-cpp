#include <functional>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "../util/debug.hpp"
#include "../../configuration.hpp"
#include "quorum.hpp"

namespace paxos { namespace detail { namespace quorum {

quorum::quorum (
   boost::asio::io_service &    io_service,
   paxos::configuration const & configuration)
   : io_service_ (io_service),
     configuration_ (configuration)
{
   /*!
     This is the client constructor, and we do not have to initialize anything here,
     since we are not a part of the quorum.
    */
}

quorum::quorum (
   boost::asio::io_service &                    io_service,
   boost::asio::ip::tcp::endpoint const &       endpoint,
   paxos::configuration const &                 configuration)
   : io_service_ (io_service),
     configuration_ (configuration),
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
quorum::our_endpoint () const
{
   return *our_endpoint_;
}


void
quorum::add (
   boost::asio::ip::tcp::endpoint const &       endpoint)
{
   servers_.insert (std::make_pair (endpoint, 
                                    detail::quorum::server (io_service_,
                                                            endpoint)));
}

detail::quorum::server &
quorum::lookup_server (
   boost::asio::ip::tcp::endpoint const &       endpoint)
{
   auto pos = servers_.find (endpoint);
   PAXOS_ASSERT (pos != servers_.end ());
   return pos->second;
}

detail::quorum::server const &
quorum::lookup_server (
   boost::asio::ip::tcp::endpoint const &       endpoint) const
{
   auto pos = servers_.find (endpoint);
   PAXOS_ASSERT (pos != servers_.end ());
   return pos->second;
}


boost::optional <boost::asio::ip::tcp::endpoint>
quorum::who_is_our_leader (
   bool allow_unknown)
{
   /*!
     We can only possibly select a leader from all live servers.
    */
   std::vector <boost::asio::ip::tcp::endpoint> live_servers = this->live_servers ();

   if (live_servers.empty () == true)
   {
      return boost::none;
   }

   std::map <boost::uuids::uuid, boost::asio::ip::tcp::endpoint> sorted;
   for (auto const & endpoint : live_servers)
   {
      detail::quorum::server & server = lookup_server (endpoint);

      if (allow_unknown == true ||
          server.has_id () == true)
      {
         PAXOS_DEBUG ("allow_unknown = " << allow_unknown << ", has id = " << server.has_id () << ", server " << endpoint << " has id " << server.id ());
         sorted[server.id ()] = endpoint;
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
   return sorted.begin ()->second;
}

std::vector <boost::asio::ip::tcp::endpoint>
quorum::live_servers ()
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

   return result;
}

void
quorum::connection_died (
   boost::asio::ip::tcp::endpoint const &       endpoint)
{
   lookup_server (endpoint).reset_connection ();
   lookup_server (endpoint).reset_id ();
}


}; }; };
