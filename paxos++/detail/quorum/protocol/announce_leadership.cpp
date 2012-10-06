#include "../../util/debug.hpp"
#include "../../command.hpp"
#include "../../parser.hpp"
#include "../quorum.hpp"

#include "announce_leadership.hpp"

namespace paxos { namespace detail { namespace quorum { namespace protocol {

/*! static */ void
announce_leadership::step1 (
   boost::asio::io_service &                    io_service,
   connection::scoped_connection::pointer       connection,
   detail::quorum::quorum &                     quorum)
{
   /*!
     Only the leader can announce leadership
    */
   PAXOS_ASSERT (quorum.who_should_be_leader () == quorum.our_endpoint ());

   /*!
     And it would be rediculous if we were a client.
    */
   PAXOS_ASSERT (quorum.our_state () != server::state_client);

   command command;
   command.set_type (command::type_leader_claim);
   command.set_host_endpoint (quorum.our_endpoint ());
   command.set_host_id (quorum.our_id ());
   
   /*!
     Writing this command to the connection will make the remote end enter
     step2 ().
    */   
   parser::write_command (connection->connection (),
                          command);

   /*!
     We're not expecting a response to the leadership announcement
    */
   connection->done ();
}

/*! static */ void
announce_leadership::step2 (
   connection::tcp_connection::pointer  connection,
   detail::command const &              command,
   detail::quorum::quorum &             quorum)
{
   if (quorum.who_should_be_leader () == command.host_endpoint ())
   {
      PAXOS_DEBUG ("we have a leader: " << command.host_endpoint ());
      /*!
        We have a new leader, make sure the quorum knows that!
       */
      quorum.new_leader (command.host_endpoint ());
   }
   else
   {
      /*!
        This means there's something terribly wrong with the quorum's state,
        let's reset everything inside the quorum to an 'unknown' state.
       */
      quorum.reset ();
   }
}


}; }; }; };
