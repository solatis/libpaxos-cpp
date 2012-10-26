#include <boost/uuid/uuid_io.hpp>

#include "../../util/debug.hpp"
#include "../../command.hpp"
#include "../../parser.hpp"
#include "../../quorum/client_view.hpp"
#include "../../tcp_connection.hpp"

#include "initiate_request.hpp"

namespace paxos { namespace detail { namespace client { namespace protocol {

/*! static */ void
initiate_request::step1 (      
   std::string const &                  byte_array,
   detail::quorum::client_view &        quorum,
   callback_type                        callback,
   queue_guard_type                     guard)
{
   boost::optional <boost::asio::ip::tcp::endpoint> leader = quorum.select_leader ();
   if (leader.is_initialized () == false)
   {
      PAXOS_DEBUG ("leader.is_initialized () == false");
      callback (detail::error_no_leader, "");
      quorum.advance_leader ();
      return;
   }

   PAXOS_DEBUG ("leader.is_initialized () == true");

   quorum::server & server = quorum.lookup_server (*leader);

   if (server.has_connection () == false)
   {
      PAXOS_DEBUG ("server.has_connection () == false");
      callback (detail::error_no_leader, "");
      quorum.advance_leader ();
      return;
   }

   PAXOS_DEBUG ("server.has_connection () == true");
   PAXOS_DEBUG ("sending request to host " << server.endpoint () << " with id = " << server.id ());

   detail::tcp_connection_ptr connection = server.connection ();
 
   /*!
     Now that we have our leader's connection, let's send it our command to initiate
     the request. 
   */
   command command;
   command.set_type (command::type_request_initiate);
   command.set_workload (byte_array);
   connection->write_command (command);

   PAXOS_INFO ("client initiating request!");

   connection->read_command (
      [connection, 
       & quorum,
       & server,
       callback,
       guard] (
          boost::optional <enum detail::error_code>     error,
          detail::command const &                       c)
      {
         if (error)
         {
            PAXOS_WARN ("client had problems communicating with leader: " << detail::to_string (*error));
            quorum.connection_died (server.endpoint ());
            quorum.advance_leader ();

            callback (*error, "");
         }
         else
         {
            quorum.lookup_server (c.host_endpoint ()).set_id (c.host_id ());
            quorum.lookup_server (c.host_endpoint ()).set_highest_proposal_id (c.highest_proposal_id ());

            switch (c.type ())
            {
                  case command::type_request_accepted:
                     PAXOS_DEBUG ("received command with workload = " << c.workload () << ", "
                                  "now calling callback!");
                     callback (boost::none, c.workload ());
                     break;
                  
                  case command::type_request_error:
                     if (c.error_code () == detail::error_no_leader)
                     {
                        quorum.advance_leader ();
                     }

                     PAXOS_WARN ("request error occured");
                     callback (c.error_code (), c.workload ());
                     break;

                  default:
                     PAXOS_UNREACHABLE ();
            };
         }
      });
}


}; }; }; };
