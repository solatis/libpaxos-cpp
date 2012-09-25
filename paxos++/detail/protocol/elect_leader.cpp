#include <iostream>
#include <vector>

#include <boost/ref.hpp>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>

#include "../../quorum.hpp"
#include "../debug.hpp"
#include "../connection_pool.hpp"
#include "protocol.hpp"
#include "elect_leader.hpp"

namespace paxos { namespace detail { namespace protocol {

elect_leader::elect_leader (
   detail::protocol::protocol & protocol)
   : quorum_ (protocol.quorum ()),
     connection_pool_ (protocol.connection_pool ())
{
}

void
elect_leader::start ()
{
   step1 ();
}

void
elect_leader::step1 ()
{
   std::vector <boost::asio::ip::tcp::endpoint> endpoints;
   quorum_.get_endpoints (std::back_inserter (endpoints));

   BOOST_FOREACH (boost::asio::ip::tcp::endpoint const & endpoint, endpoints)
   {
      if (endpoint == quorum_.self ())
      {
         PAXOS_DEBUG ("skipping self: " << endpoint);
         continue;
      }

      tcp_connection & new_connection = connection_pool_.create ();
      
      new_connection.socket ().async_connect (endpoint,
                                              boost::bind (&elect_leader::step2,
                                                           this,
                                                           boost::ref (new_connection),
                                                           boost::asio::placeholders::error));
   }
}

void
elect_leader::step2 (
   tcp_connection &                             connection,
   boost::system::error_code const &            error)
{

   if (error)
   {
      PAXOS_WARN ("An error occured while establishing a connection: " << error.message ());
      return;
   }

   PAXOS_DEBUG ("Connection established!");
}

}; }; };
