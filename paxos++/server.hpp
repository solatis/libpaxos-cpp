/*!
  Copyright (c) 2012, Leon Mergen, all rights reserved.
 */

#ifndef LIBPAXOS_CPP_SERVER_HPP
#define LIBPAXOS_CPP_SERVER_HPP

#include <stdint.h>

#include <boost/asio.hpp>
#include <boost/uuid/uuid.hpp>

#include "detail/quorum.hpp"
#include "detail/tcp_connection.hpp"
#include "detail/protocol/protocol.hpp"

namespace boost { namespace asio { namespace ip {
class address;
}; }; };

namespace paxos {
class quorum;
};

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
   typedef detail::protocol::protocol::workload_processor_callback_type callback_type;


public:
   /*!
     \brief Opens socket to listen on port
     \param io_service  Boost.Asio io_service object, which represents the link to the OS'es i/o services
     \param address     Interface/ip to listen for new connections
     \param port        Port to listen for new connections
     \param quorum      The quorum of servers we are a part of.
     \param callback    Callback used to process workload

     Automatically calls quorum.we_are () to ensure the quorum knows the reference to the server.
   */
   server (
      boost::asio::io_service &         io_service,
      boost::asio::ip::address const &  address,
      uint16_t                          port,
      quorum const &                    quorum,
      callback_type const &             callback);
   
   /*!
     \brief Stops listening for new connections
    */
   void
   close ();

private:

   void
   accept ();

   void
   handle_accept (
      detail::tcp_connection::pointer   new_connection,
      boost::system::error_code const & error);


private:

   /*!
     \brief Uniquely identifies us within the quorum.
    */
   boost::uuids::uuid                   uuid_;
   boost::asio::ip::tcp::acceptor       acceptor_;

   detail::quorum                       quorum_;
   detail::protocol::protocol           protocol_;
};

}

#endif  //! LIBPAXOS_CPP_SERVER_HPP
