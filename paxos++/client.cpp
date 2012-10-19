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
   throw ()
{
   boost::shared_ptr <std::promise <std::string> > promise (
      new std::promise <std::string> ());

   this->do_request (promise,
                     byte_array,
                     retries);
   
   return promise->get_future ();
}

void
client::do_request (
   boost::shared_ptr <std::promise <std::string> >      promise,
   std::string const &                                  byte_array,
   uint16_t                                             retries)
{
   /*!
     Throws exception if quorum is not ready yet
   */
   auto callback = 
      [this,
       promise,
       byte_array,
       retries] (
         boost::optional <enum error_code>        error,
         std::string const &                      response)
      {
         if (error)
         {
            if (retries > 0)
            {
               boost::shared_ptr <boost::asio::deadline_timer> timer (
                  new boost::asio::deadline_timer (this->io_service_));

               timer->expires_from_now (
                  boost::posix_time::milliseconds (500));
               timer->async_wait (
                  [this, 
                   promise,
                   byte_array,
                   retries,
                   timer]
                  (boost::system::error_code const & error)
               {
                  if (!error)
                  {
                     this->do_request (promise,
                                       byte_array,
                                       retries - 1);
                  }
               });
            }
            else
            {
               PAXOS_WARN ("Caught error in response to client request: " << paxos::to_string (*error));
               promise->set_exception (std::make_exception_ptr (exception::request_error ()));
            }
         }
         else
         {
            promise->set_value (response);
         }
      };

   request_queue_.push (
      {byte_array, quorum_, callback});
}

};
