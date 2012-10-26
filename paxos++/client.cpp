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
   boost::asio::io_service &    io_service)
   : io_service_ (io_service),
     quorum_ (io_service),
     request_queue_ (
        []
        (detail::client::protocol::request const &                                              request,
         detail::request_queue::queue <detail::client::protocol::request>::guard::pointer       guard)
        {
           detail::client::protocol::initiate_request::step1 (request.byte_array_,
                                                              request.quorum_,
                                                              request.callback_,
                                                              guard);
        })
{
}

client::~client ()
{
   io_thread_.stop ();
}

void
client::add (
   std::initializer_list <std::pair <std::string, uint16_t> > const &        servers)
{
   for (auto const & i : servers)
   {
      this->add (i.first,
                 i.second);
   }
}

void
client::add (
   std::string const &  host,
   uint16_t             port)
{
   quorum_.add (
      boost::asio::ip::tcp::endpoint (
         boost::asio::ip::address::from_string (host), port));
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
   request_queue_.push (
      {byte_array, quorum_, 

            /*!
              This callback handles the response we get from the paxos leader. It automatically
              waits & retries in case of an error.
            */
            [this,
             promise,
             byte_array,
             retries] (
                boost::optional <enum detail::error_code>       error,
                std::string const &                             response)
            {
               if (error)
               {
                  PAXOS_WARN ("Caught error in response to client request: " << detail::to_string (*error));

                  if (retries > 0)
                  {
                     /*!
                       We still have retries left and an error occured, let's wait a short while
                       and retry to see if it automatically recovers.

                       We declare the timer in a shared_ptr because it shouldn't go out of scope.
                     */
                     boost::shared_ptr <boost::asio::deadline_timer> timer (
                        new boost::asio::deadline_timer (this->io_service_));

                     /*!
                       \todo Make this configurable
                     */
                     timer->expires_from_now (
                        boost::posix_time::milliseconds (500));

                     /*!
                       Here we define our callback-within-a-callback: we need to do this since we
                       cannot block, since we're inside the control thread that handles the io_service_
                       processing.

                       What we will do here is do an async wait on the timer. Note how we grab the 
                       timer's shared_ptr as a parameter and do not do anything with it; this is just
                       to ensure the timer stays alive.
                     */
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
                              /*!
                                The timer wasn't cancelled, let's perform the retry.
                              */
                              this->do_request (promise,
                                                byte_array,
                                                retries - 1);
                           }
                        });
                  }
                  else
                  {
                     /*!
                       We do not have any retries left, let's set the exception.
                      */

                     switch (*error)
                     {
                           case detail::error_no_leader:
                              promise->set_exception (
                                 std::make_exception_ptr (
                                    exception::no_leader ()));
                              break;

                           case detail::error_incorrect_proposal:
                              promise->set_exception (
                                 std::make_exception_ptr (
                                    exception::incorrect_proposal ()));
                              break;

                           case detail::error_inconsistent_response:
                              promise->set_exception (
                                 std::make_exception_ptr (
                                    exception::inconsistent_response ()));
                              break;

                           case detail::error_connection_close:
                              promise->set_exception (
                                 std::make_exception_ptr (
                                    exception::connection_close ()));
                              break;

                           case detail::error_no_majority:
                              promise->set_exception (
                                 std::make_exception_ptr (
                                    exception::no_majority ()));
                              break;
                              
                           default:
                              PAXOS_UNREACHABLE ();
                     };
                  }
               }
               else
               {
                  /*!
                    No errors occured, so we have an actual return value.
                   */
                  PAXOS_DEBUG ("client setting promise response: " << response);
                  promise->set_value (response);
               }
            }
      });
}

};
