#include <iostream>
#include <vector>

#include <boost/ref.hpp>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>

#include "../util/debug.hpp"
#include "../../quorum.hpp"
#include "../connection_pool.hpp"

#include "command.hpp"
#include "protocol.hpp"
#include "elect_leader.hpp"

namespace paxos { namespace detail { namespace protocol {

elect_leader::elect_leader (
   detail::protocol::protocol & protocol)
   : protocol_ (protocol)
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
   protocol_.quorum ().get_endpoints (std::back_inserter (endpoints));
   
   for (boost::asio::ip::tcp::endpoint const & endpoint : endpoints)
   {
      if (endpoint == protocol_.quorum ().self ().endpoint_)
      {
         PAXOS_DEBUG ("skipping self: " << endpoint);
         continue;
      }

      tcp_connection & new_connection = protocol_.connection_pool ().create ();
      
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

   command command;
   command.set_type (command::start_election);
   command.set_host_id (protocol_.quorum ().self ().id_);

   PAXOS_DEBUG ("Connection established!");

   protocol_.write_command (command,
                            connection);

   //! Timeout of 3 seconds
   connection.start_timeout (boost::posix_time::milliseconds (3000));

   protocol_.read_command (connection,
                           boost::bind (&elect_leader::step3,
                                        this,
                                        boost::ref (connection),
                                        _1));
}


void
elect_leader::step3 (
   tcp_connection &     connection,
   command const &      command)
{
   PAXOS_DEBUG ("received command!");
}

}; }; };
