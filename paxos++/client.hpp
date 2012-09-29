/*!
  Copyright (c) 2012, Leon Mergen, all rights reserved.
 */

#ifndef LIBPAXOS_CPP_CLIENT_HPP
#define LIBPAXOS_CPP_CLIENT_HPP

#include <boost/function.hpp>

#include "exception.hpp"
#include "detail/quorum.hpp"
#include "detail/protocol/protocol.hpp"

namespace boost { namespace asio {
class io_service;
}; };

namespace paxos {
class quorum;
};

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
     \brief Callback type for send ()
    */
   typedef boost::function <void (boost::asio::ip::tcp::endpoint const &,
                                  std::string const &)> callback_type;

   /*!
     \brief Opens client and starts quorum discovery process
     \param io_service  Boost.Asio io_service object, which represents the link to the OS'es i/o services
     \param quorum      The quorum of clients we are a part of.
   */
   client (
      boost::asio::io_service &         io_service,
      quorum const &                    quorum);


   /*!
     \brief Sends data to entire quorum and call callback with result
     \param byte_array  Data to sent. Binary-safe.
     \param callback    For each server in the quorum, called with response 
                        to the command that has been received
     \param retries     Amount of times to retry in case the quorum is not yet ready
     \throws Can throw a 'not_ready_exception' when the quorum is in an inconsistent state
    */
   void
   async_send (
      std::string const &       byte_array,
      callback_type             callback,
      uint16_t                  retries = 1) throw (paxos::not_ready_exception);

   /*!
     \brief Sends data to entire quorum and blocks until command is completed
     \param byte_array  Data to sent. Binary-safe.
     \param retries     Amount of times to retry in case the quorum is not yet ready
     \throws Can throw a 'not_ready_exception' when the quorum is in an inconsistent state
    */
   void
   send (
      std::string const &       byte_array,
      uint16_t                  retries = 1) throw (paxos::not_ready_exception);


private:

   detail::quorum                       quorum_;
   detail::protocol::protocol           protocol_;
};

}

#endif  //! LIBPAXOS_CPP_CLIENT_HPP
