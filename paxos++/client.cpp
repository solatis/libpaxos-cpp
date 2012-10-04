#include <boost/thread/condition.hpp>

#include <boost/asio/placeholders.hpp>
#include <boost/asio/deadline_timer.hpp>

#include "detail/util/debug.hpp"
#include "detail/client/protocol/initiate_request.hpp"

#include "configuration.hpp"
#include "exception/exception.hpp"
#include "client.hpp"

namespace paxos {

client::client ()
   : client (io_thread_.io_service ())
{
   io_thread_.launch ();
}

client::client (
   boost::asio::io_service &            io_service)
   : io_service_ (io_service),
     quorum_ (io_service)
{
}

client::~client ()
{
   io_thread_.stop ();
}

void
client::start ()
{
   quorum_.bootstrap ();
}

void
client::add (
   std::string const &  server,
   uint16_t             port)
{
   quorum_.add (
      boost::asio::ip::tcp::endpoint (
         boost::asio::ip::address::from_string (server), port));
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
   do
   {
      try
      {
         /*!
           Throws exception if quorum is not ready yet
          */
         detail::client::protocol::initiate_request::step1 (byte_array,
                                                            quorum_,
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
         boost::asio::deadline_timer timer (io_service_);
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
   throw (exception::not_ready,
          exception::request_error)
{
   boost::mutex lock;
   boost::unique_lock <boost::mutex> guard (lock);

   boost::condition response_received;   

   std::string output_response;

   callback_type callback = 
      [& response_received,
       & output_response] (boost::optional <enum error_code> error_code,
                           std::string const & input_response)
   {
      PAXOS_CHECK_THROW (
         error_code.is_initialized () == true, exception::request_error ());                   

      output_response = input_response;
      //! We should wait until we have received responses for all nodes in quorum
      response_received.notify_one ();
   };

   async_send (byte_array, callback, retries);


   response_received.wait (guard);

   return output_response;
}


};
