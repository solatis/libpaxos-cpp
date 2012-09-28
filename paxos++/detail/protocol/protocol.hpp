/*!
  Copyright (c) 2012, Leon Mergen, all rights reserved.
 */

#ifndef LIBPAXOS_CPP_DETAIL_PROTOCOL_PROTOCOL_HPP
#define LIBPAXOS_CPP_DETAIL_PROTOCOL_PROTOCOL_HPP

#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>
#include <boost/asio/deadline_timer.hpp>

#include "../tcp_connection.hpp"
#include "handshake.hpp"
#include "elect_leader.hpp"
#include "announce_leadership.hpp"

namespace boost { namespace asio {
class io_service;
}; };

namespace paxos { namespace detail {
class quorum;
}; };

namespace paxos { namespace detail { namespace protocol {
class command;
}; }; };

namespace paxos { namespace detail { namespace protocol {

/*!
  \brief Entry point for communication within quorum
 */
class protocol
{
private:
   typedef boost::function <void (command const &)> read_command_callback_type;
   
public:

   /*!
     \brief Constructor
     \param connection_pool     Used to create new connections
     \param quorum              Quorum of servers we're communicating with.
    */
   protocol (
      boost::asio::io_service &         io_service,
      detail::quorum &                  quorum);


   /*!
     \brief Access to the underlying I/O service
    */
   boost::asio::io_service &
   io_service ();

   /*!
     \brief Access to the underlying quorum
    */
   detail::quorum &
   quorum ();

   /*!
     \brief Starts leader election
    */
   void
   bootstrap ();


   /*!
     \brief Periodically called to validate the quorum's health
    */
   void
   heartbeat ();

   /*!
     \brief Called by local_server when a new connection arrives.
    */
   void
   new_connection (
      tcp_connection::pointer   connection);

   /*!
     \brief Callback function for incoming command by conneciton
    */
   void
   handle_command (
      tcp_connection::pointer   connection,
      command const &           command);

   /*!
     \brief Serializes a protocolbuffers command to a string and sends it over the wire
    */
   void
   write_command (
      command const &           command,
      tcp_connection::pointer   output);

   /*!
     \brief Reads binary data from wire and parses command out of it
     \note  If any timeouts are set on <connection>, these timers are automatically cancelled
    */
   void
   read_command (
      tcp_connection::pointer                   connection,
      read_command_callback_type const &        callback);

private:

   void
   read_command_parse_size (
      tcp_connection::pointer                           connection,
      boost::system::error_code const &                 error,
      size_t                                            bytes_transferred,
      boost::shared_array <char>                        buffer,
      boost::shared_ptr <read_command_callback_type>    callback);

   void
   read_command_parse_command (
      tcp_connection::pointer                           connection,
      boost::system::error_code const &                 error,
      size_t                                            bytes_transferred,
      boost::shared_array <char>                        buffer,
      boost::shared_ptr <read_command_callback_type>    callback);

private:

   boost::asio::io_service &            io_service_;
   boost::asio::deadline_timer          heartbeat_timer_;

   detail::quorum &                     quorum_;

   handshake                            handshake_;
   elect_leader                         elect_leader_;
   announce_leadership                  announce_leadership_;
};

}; }; };

#endif //! LIBPAXOS_CPP_DETAIL_PROTOCOL_PROTOCOL_HPP

