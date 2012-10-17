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
     request_queue_ (
        []
        (detail::client::protocol::request const &                                              request,
         detail::request_queue::queue <detail::client::protocol::request>::guard::pointer       guard)
        {
           detail::client::protocol::initiate_request::step1 (request.byte_array_,
                                                              request.quorum_,
                                                              request.callback_,
                                                              guard);
        }),

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

void
client::wait_until_quorum_ready (
   uint16_t     attempts) const
   throw (exception::not_ready)
{
   while (--attempts)
   {
      if (quorum_.we_have_a_leader_connection () == true
          && quorum_.has_majority (quorum_.who_is_our_leader ()) == true)
      {
         return;
      }

      boost::asio::deadline_timer timer (io_service_);
      timer.expires_from_now (
         boost::posix_time::milliseconds (heartbeat_interval_));
      timer.wait ();
   }

   PAXOS_THROW (exception::not_ready ());
}


std::future <std::string>
client::send (
   std::string const &  byte_array)
   throw (exception::not_ready)
{
   boost::shared_ptr <std::promise <std::string> > promise (
      new std::promise <std::string> ());

   /*!
     Throws exception if quorum is not ready yet
   */
   auto callback = 
      [promise] (
         boost::optional <enum error_code>        error,
         std::string const &                      response)
      {
         if (error)
         {
            PAXOS_WARN ("Caught error in response to client request: " << paxos::to_string (*error));
            promise->set_exception (std::make_exception_ptr (exception::request_error ()));
         }
         else
         {
            promise->set_value (response);
         }
      };

   request_queue_.push (
      {byte_array, quorum_, callback});
   
   return promise->get_future ();
}

};
