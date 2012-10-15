#include <boost/thread/condition.hpp>

#include <boost/asio/placeholders.hpp>
#include <boost/asio/deadline_timer.hpp>

#include "detail/util/debug.hpp"
#include "detail/client/protocol/initiate_request.hpp"

#include "configuration.hpp"
#include "exception/exception.hpp"
#include "client.hpp"

namespace paxos {

client::client (
   paxos::configuration configuration)
   : client (io_thread_.io_service (),
             configuration)
{
   io_thread_.launch ();
}

client::client (
   boost::asio::io_service &    io_service,
   paxos::configuration         configuration)
   : io_service_ (io_service),
     quorum_ (io_service,
              configuration),
     heartbeat_interval_ (configuration.heartbeat_interval ())
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


std::future <std::string>
client::send (
   std::string const &  byte_array,
   uint16_t             retries)
   throw (exception::not_ready)
{
   boost::shared_ptr <std::promise <std::string> > promise (
      new std::promise <std::string> ());

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
         detail::client::protocol::initiate_request::step1 (
            byte_array,
            quorum_,
            [promise] (
               boost::optional <enum error_code>        error_code,
               std::string const &                      response)
            {
               if (error_code.is_initialized () == true)
               {
                  promise->set_exception (std::make_exception_ptr (exception::request_error ()));
               }
               else
               {
                  promise->set_value (response);
               }
            });

         return promise->get_future ();
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
            boost::posix_time::milliseconds (heartbeat_interval_));
         timer.wait ();
      }
   } while (--retries > 0);


   PAXOS_THROW (exception::not_ready ());
}


};
