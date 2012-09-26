#include <iostream>
#include <vector>

#include <boost/ref.hpp>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>

#include "../util/debug.hpp"
#include "../../quorum.hpp"
#include "../connection_pool.hpp"
#include "pb/command.pb.h"
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
      if (endpoint == quorum_.self ().endpoint_)
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

   pb::command command;
   command.set_type (pb::START_ELECTION);
   command.set_host_id (quorum_.self ().id_);

   PAXOS_DEBUG ("Connection established!");

   protocol::write_command (command,
                            connection);

   protocol::read_command (connection,
                           boost::bind (&elect_leader::step3,
                                        this,
                                        boost::ref (connection),
                                        _1));
}


void
elect_leader::step3 (
   tcp_connection &     connection,
   pb::command const &  command)
{
   PAXOS_DEBUG ("received command!");
}

}; }; };
