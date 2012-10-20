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

namespace paxos {

/*!
  \brief Represents the local paxos server.
  \related quorum

  This is the main entry point which communications within the paxos quorum take place. It
  hooks itself onto the io_service, and will start accepting connections as soon as 
  io_service.handle_events () is called by an outside thread. This thread can, however, be
  created by the server itself. 

  \b Example \b usage:
  \code{.cpp} 

  paxos::server::callback_type callback = 
     [& response_count](std::string const & byte_array) -> std::string
     {
        return byte_array;
     };  

  paxos::server server1 ("127.0.0.1", 1337, callback);
  paxos::server server2 ("127.0.0.1", 1338, callback);

  server1.add ({{"127.0.0.1", 1337}, {"127.0.0.1", 1338}});
  server2.add ({{"127.0.0.1", 1337}, {"127.0.0.1", 1338}});

  \endcode
  
 */
class server
{
public:

   typedef detail::paxos_context::processor_type  callback_type;

public:

   /*!
     \brief Opens socket to listen on port
     \param server        Hostname we're listening at to new connections
     \param port          Port we're listening at to new connections
     \param callback      Callback used to process workload
     \param configuration (Optional) Runtime configuration

     This constructor launches its own background thread with i/o context.
   */
   server (
      std::string const &               server,
      uint16_t                          port,
      callback_type const &             callback,
      paxos::configuration              configuration = paxos::configuration ());
   
   /*!
     \brief Opens socket to listen on port
     \param io_service    Boost.Asio io_service object, which represents the link to the OS'es i/o services
     \param server        Hostname we're listening at to new connections
     \param port          Port we're listening at to new connections
     \param callback      Callback used to process workload
     \param configuration (Optional) Runtime configuration
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
     \brief Helper function of add (), which adds a whole list at once
     \param servers List of pairs of server/ports to connect to
    */
   void
   add (
      std::initializer_list <std::pair <std::string, uint16_t> > const &        servers);

   /*!
     \brief Stops listening for new connections, closes all existing connections and stops
            the background thread (if any)
    */
   void
   stop ();

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
