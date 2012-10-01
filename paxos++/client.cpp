#include <boost/thread/condition.hpp>
#include <boost/asio/deadline_timer.hpp>

#include "detail/util/debug.hpp"

#include "configuration.hpp"
#include "exception/exception.hpp"
#include "quorum.hpp"
#include "client.hpp"

namespace paxos {

client::client (
   boost::asio::io_service &            io_service,
   quorum const &                       quorum)
   : quorum_ (quorum.quorum_),
     protocol_ (io_service,
                quorum_,
                [](std::string const &) -> std::string 
                {
                   PAXOS_UNREACHABLE ();
                })
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
   throw (exception::not_ready)
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
         PAXOS_CHECK_THROW (quorum_.has_leader () == false, exception::not_ready ());

         detail::remote_server & leader = quorum_.lookup (quorum_.current_leader ());

         /*!
           And if there is no connection established with the leader, we're not ready yet
           either. This should usually be resolved within the next heartbeat.
          */
         PAXOS_CHECK_THROW (leader.has_connection () == false, exception::not_ready ());

         /*!
           And at this point, we have a connection to the leader which is alive and kicking!
           Let's issue the request.
          */
         protocol_.initiate_request (leader.connection (),
                                     byte_array,
                                     callback);
         return;
      }
      catch (exception::not_ready const & e)
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


   PAXOS_THROW (exception::not_ready ());
}

std::string
client::send (
   std::string const &  byte_array,
   uint16_t             retries)
   throw (exception::not_ready)
{
   boost::mutex lock;
   boost::unique_lock <boost::mutex> guard (lock);

   boost::condition response_received;   

   std::string output_response;

   callback_type callback = 
      [& response_received,
       & output_response] (std::string const & input_response)
   {
      output_response = input_response;
      //! We should wait until we have received responses for all nodes in quorum
      response_received.notify_one ();
   };

   async_send (byte_array, callback, retries);


   response_received.wait (guard);

   return output_response;
}


};
