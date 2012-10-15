/*!
  Copyright (c) 2012, Leon Mergen, all rights reserved.
 */

#ifndef LIBPAXOS_CPP_SERVER_HPP
#define LIBPAXOS_CPP_SERVER_HPP

#include <stdint.h>
#include <string>

#include <boost/asio/ip/tcp.hpp>

#include "detail/strategy/basic_paxos/factory.hpp"

#include "detail/io_thread.hpp"
#include "detail/paxos_context.hpp"
#include "detail/quorum/quorum.hpp"
#include "detail/tcp_connection_fwd.hpp"

#include "configuration.hpp"

namespace boost { namespace asio { namespace ip {
class address;
}; }; };

namespace paxos {

/*!
  \brief Represents the local paxos server.
  \related quorum

  This is the main entry point which communications within the paxos quorum take place. It
  hooks itself onto the io_service, and will start accepting connections as soon as 
  io_service.handle_events () is called by an outside thread.

  \b Example \b usage:
  \code{.cpp}
 
  paxos::quorum quorum;
  quorum.add (boost::asio::ip::address_v4::from_string ("127.0.0.1"), 1337);
  quorum.add (boost::asio::ip::address_v4::from_string ("127.0.0.1"), 1338);

  paxos::server server1 (io_service, 
                         boost::asio::ip::address_v4::from_string ("127.0.0.1"), 1337,
                         quorum);
  paxos::server server2 (io_service, 
                         boost::asio::ip::address_v4::from_string ("127.0.0.1"), 1339,
                         quorum);

  io_service.run ();
  \endcode
  
 */
class server
{
public:

   typedef detail::paxos_context::processor_type  callback_type;

public:

   /*!
     \brief Opens socket to listen on port
     \param io_service  Boost.Asio io_service object, which represents the link to the OS'es i/o services
     \param endpoint    Endpoint where we're listening to new connections
     \param callback    Callback used to process workload

     This constructor launches its own background thread with i/o context. Automatically calls 
     quorum.we_are () to ensure the quorum knows the reference to the server.
   */
   server (
      std::string const &               server,
      uint16_t                          port,
      callback_type const &             callback,
      paxos::configuration              configuration = paxos::configuration ());
   
   /*!
     \brief Opens socket to listen on port
     \param io_service  Boost.Asio io_service object, which represents the link to the OS'es i/o services
     \param endpoint    Endpoint where we're listening to new connections
     \param callback    Callback used to process workload

     Automatically calls quorum.we_are () to ensure the quorum knows the reference to the server.
   */
   server (
      boost::asio::io_service &         io_service,
      std::string const &               server,
      uint16_t                          port,
      callback_type const &             callback,
      paxos::configuration              configuration = paxos::configuration ());

   /*!
     \brief Destructor
     
     Gracefully closes the background io thread, if any.
    */
   ~server ();
   

   /*!
     \brief Adds server to quorum registered the server is part of
     \param server      Hostname of server to connect to
     \param port        Port of server to connect to
    */
   void
   add (
      std::string const &                       server,
      uint16_t                                  port);

   /*!
     \brief Bootstraps the quorum and starts connecting to other nodes
    */
   void
   start ();

   /*!
     \brief Stops listening for new connections, closes all existing connections and stops the background thread (if any)
    */
   void
   close ();

private:

   void
   accept ();

   void
   handle_accept (
      detail::tcp_connection_ptr        new_connection,
      boost::system::error_code const & error);

private:
   
   detail::io_thread                    io_thread_;
   boost::asio::ip::tcp::acceptor       acceptor_;
   detail::quorum::quorum               quorum_;
   detail::paxos_context                state_;
};

}

#endif  //! LIBPAXOS_CPP_SERVER_HPP
