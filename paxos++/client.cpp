#include <boost/asio/deadline_timer.hpp>

#include "detail/util/debug.hpp"

#include "configuration.hpp"
#include "exception.hpp"
#include "quorum.hpp"
#include "client.hpp"

namespace paxos {

client::client (
   boost::asio::io_service &            io_service,
   quorum const &                       quorum)
   : quorum_ (quorum.quorum_),
     protocol_ (io_service,
                quorum_)
{
   //! Bootstrap our protocol and quorum by discovering who the leader is
   protocol_.bootstrap ();

   //! Ensure that our quorum knows that we are a client
   quorum_.adjust_our_state (
      detail::remote_server::state_client);
}


void
client::async_send (
   std::string const &  byte_array,
   callback_type        callback,
   uint16_t             retries)
   throw (paxos::not_ready_exception)
{
   /*!
     We are a client, we can NEVER EVER be a leader.
    */
   PAXOS_ASSERT (quorum_.we_are_the_leader () == false);

   do
   {
      try
      {
         /*!
           If the quorum doesn't have a leader, we're not ready yet.
          */
         PAXOS_CHECK_THROW (quorum_.has_leader () == false, paxos::not_ready_exception ());

         detail::remote_server & leader = quorum_.lookup (quorum_.current_leader ());

         /*!
           And if there is no connection established with the leader, we're not ready yet
           either. This should usually be resolved within the next heartbeat.
          */
         PAXOS_CHECK_THROW (leader.has_connection () == false, paxos::not_ready_exception ());

         /*!
           And at this point, we have a connection to the leader which is alive and kicking!
           Let's issue the request.
          */
         protocol_.initiate_request (leader.connection (),
                                     byte_array);
         return;
      }
      catch (paxos::not_ready_exception const & e)
      {
         PAXOS_WARN ("quorum is not yet ready, retries left = " << retries);

         /*!
           It makes sense to wait for configuration::heartbeat_interval amount of time,
           since that means that each retry, at least one heartbeat has occured.
          */
         boost::asio::deadline_timer timer (protocol_.io_service ());
         timer.expires_from_now (
            boost::posix_time::milliseconds (configuration::heartbeat_interval));
         timer.wait ();
      }
   } while (--retries > 0);


   PAXOS_THROW (paxos::not_ready_exception ());
}


};
