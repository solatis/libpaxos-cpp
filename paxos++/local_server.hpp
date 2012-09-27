/*!
  Copyright (c) 2012, Leon Mergen, all rights reserved.
 */

#ifndef LIBPAXOS_CPP_LOCAL_SERVER_HPP
#define LIBPAXOS_CPP_LOCAL_SERVER_HPP

#include <stdint.h>

#include <boost/asio.hpp>
#include <boost/uuid/uuid.hpp>

#include "quorum.hpp"
#include "detail/tcp_connection.hpp"
#include "detail/protocol/protocol.hpp"

namespace boost { namespace asio { namespace ip {
class address;
}; }; };

namespace paxos {

/*!
  \brief Represents the local paxos server.

  This is the main entry point which communications within the paxos quorum take place.
  The open () function will open the server socket and makes it ready to accept connections
  from the quorum. 

  However, it does not process any messages until handle_events () is called. 
 */
class local_server
{
public:

   /*!
     \brief Opens socket to listen on port
     \param io_service  Boost.Asio io_service object, which represents the link to the OS'es i/o services
     \param address     Interface/ip to listen for new connections
     \param port        Port to listen for new connections
     \param quorum      The quorum of servers we are a part of.

     Automatically calls quorum.we_are () to ensure the quorum knows the reference to the server.
   */
   local_server (
      boost::asio::io_service &         io_service,
      boost::asio::ip::address const &  address,
      uint16_t                          port,
      quorum const &                    quorum);
   
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

   quorum                               quorum_;
   detail::protocol::protocol           protocol_;
};

}

#endif  //! LIBPAXOS_CPP_LOCAL_SERVER_HPP
