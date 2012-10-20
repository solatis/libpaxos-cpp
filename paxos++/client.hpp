/*!
  Copyright (c) 2012, Leon Mergen, all rights reserved.
 */

#ifndef LIBPAXOS_CPP_CLIENT_HPP
#define LIBPAXOS_CPP_CLIENT_HPP

#include <future>
#include <initializer_list>

#include "detail/io_thread.hpp"
#include "detail/quorum/quorum.hpp"
#include "detail/request_queue/queue.hpp"
#include "detail/client/protocol/request.hpp"

#include "configuration.hpp"

namespace paxos {

/*!
  \brief Represents a paxos client.

  Clients that want to talk to the Paxos quorum should use this class. It is
  very similar to server, in that it needs the same quorum and periodically
  handshakes with all servers inside the quorum to determine their state.

  \b Example \b usage:
  \code{.cpp}

  paxos::client client;
  client.add ({{"127.0.0.1", 1337}, {"127.0.0.1", 1338}, {"127.0.0.1", 1339}});

  std::future <std::string> future = client.send ("foo");
  std::string result = future.get ();

  \endcode
  
 */
class client
{
public:

   /*!
     \brief Opens client

     This constructor launches its own background thread with i/o context
    */
   client (
      paxos::configuration              configuration = paxos::configuration ());

   /*!
     \brief Opens client
     \param io_service  Boost.Asio io_service object, which represents the link to the OS'es i/o services
   */
   client (
      boost::asio::io_service &         io_service,
      paxos::configuration              configuration = paxos::configuration ());

   /*!
     \brief Destructor
     
     Gracefully closes the background io thread, if any.
    */
   ~client ();

   /*!
     \brief Adds server to quorum registered within the client
     \param server      Hostname of server to connect to
     \param port        Port of server to connect to
    */
   void
   add (
      std::string const &       server,
      uint16_t                  port);

   /*!
     \brief Helper function of add (), which adds a whole list at once
     \param servers List of pairs of server/ports to connect to
    */
   void
   add (
      std::initializer_list <std::pair <std::string, uint16_t> > const &        servers);

   /*!
     \brief Sends data to entire quorum and call callback with result
     \param byte_array  Data to sent. Binary-safe.
     \returns Returns a future to the result
    */
   std::future <std::string>
   send (
      std::string const &       byte_array,
      uint16_t                  retries = 10) 
      throw ();

private:

   void
   do_request (
      boost::shared_ptr <std::promise <std::string> >   promise,
      std::string const &                               byte_array,
      uint16_t                                          retries);

private:


   detail::io_thread                                                    io_thread_;
   boost::asio::io_service &                                            io_service_;
   detail::quorum::quorum                                               quorum_;
   detail::request_queue::queue <detail::client::protocol::request>     request_queue_;

};

}

#endif  //! LIBPAXOS_CPP_CLIENT_HPP
