/*!
  Copyright (c) 2012, Leon Mergen, all rights reserved.
 */

#ifndef LIBPAXOS_CPP_CLIENT_HPP
#define LIBPAXOS_CPP_CLIENT_HPP

#include <boost/function.hpp>

#include "exception/exception.hpp"
#include "detail/quorum/quorum.hpp"
#include "detail/client/protocol/initiate_request.hpp"

#include "error.hpp"

namespace boost { namespace asio {
class io_service;
}; };

namespace paxos {

/*!
  \brief Represents a paxos client.
  \related quorum

  Clients that want to talk to the Paxos quorum should use this class. It is
  very similar to server, in that it needs the same quorum and periodically
  handshakes with all servers inside the quorum to determine their state.

  \b Example \b usage:
  \code{.cpp}
 
  paxos::quorum quorum;
  quorum.add (boost::asio::ip::address_v4::from_string ("127.0.0.1"), 1337);
  quorum.add (boost::asio::ip::address_v4::from_string ("127.0.0.1"), 1338);

  paxos::client client (io_service, quorum);
  client.send ("foo",
               [] 
               (boost::asio::ip::tcp::endpoint const &      host,
                std::string const &                         response){});

  io_service.run ();
  \endcode
  
 */
class client
{
public:

   /*!
     \brief Callback type for async_send ()
    */
   typedef detail::client::protocol::initiate_request::callback_type    callback_type;

   /*!
     \brief Opens client
     \param io_service  Boost.Asio io_service object, which represents the link to the OS'es i/o services
   */
   client (
      boost::asio::io_service &         io_service);

   /*!
     \brief Adds server to quorum registered within the client
     \param endpoint    Endpoint where the node listens at
    */
   void
   add (
      boost::asio::ip::tcp::endpoint const &    endpoint);

   /*!
     \brief Bootstraps the quorum and starts connecting to other nodes
    */
   void
   start ();

   /*!
     \brief Sends data to entire quorum and call callback with result
     \param byte_array  Data to sent. Binary-safe.
     \param callback    For each server in the quorum, called with response 
                        to the command that has been received
     \param retries     Amount of times to retry in case the quorum is not yet ready
     \throws Can throw a 'not_ready' exception when the quorum is in an inconsistent state
    */
   void
   async_send (
      std::string const &       byte_array,
      callback_type             callback,
      uint16_t                  retries = 1) throw (exception::not_ready);

   /*!
     \brief Sends data to entire quorum and blocks until command is completed
     \param byte_array  Data to sent. Binary-safe.
     \param retries     Amount of times to retry in case the quorum is not yet ready
     \returns Returns response string returned by all servers
     \throws Can throw a 'not_ready_exception' when the quorum is in an inconsistent state
    */
   std::string
   send (
      std::string const &       byte_array,
      uint16_t                  retries = 1) throw (exception::not_ready,
                                                    exception::request_error);


private:

   boost::asio::io_service &    io_service_;
   detail::quorum::quorum       quorum_;

};

}

#endif  //! LIBPAXOS_CPP_CLIENT_HPP
