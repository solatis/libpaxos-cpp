/*!
  Copyright (c) 2012, Leon Mergen, all rights reserved.
 */

#ifndef LIBPAXOS_CPP_DETAIL_PROTOCOL_PROTOCOL_HPP
#define LIBPAXOS_CPP_DETAIL_PROTOCOL_PROTOCOL_HPP

#include <string>

#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>
#include <boost/asio/deadline_timer.hpp>

#include "../tcp_connection.hpp"
#include "handshake.hpp"
#include "elect_leader.hpp"
#include "announce_leadership.hpp"
#include "basic_paxos.hpp"

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
public:
   typedef boost::function <std::string (std::string const &)>  workload_processor_callback_type;

private:
   typedef boost::function <void (command const &)>             read_command_callback_type;
   
public:

   /*!
     \brief Constructor
     \param io_service  Link to the OS'es I/O services
     \param quorum      Quorum of servers we're communicating with.
     \param callback    Callback to process workload
    */
   protocol (
      boost::asio::io_service &                 io_service,
      detail::quorum &                          quorum,
      workload_processor_callback_type const &  callback);


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
     \brief Called by server when a new connection arrives.
    */
   void
   new_connection (
      tcp_connection::pointer   connection);


   /*!
     \brief Called by client when a new request should be sent.
    */
   void
   initiate_request (
      tcp_connection::pointer           leader_connection,
      std::string const &               byte_array,
      basic_paxos::client_callback_type callback);


   /*!
     \brief Serializes a protocolbuffers command to a string and sends it over the wire
    */
   void
   write_command (
      command const &           command,
      tcp_connection::pointer   output);

   /*!
     \brief Reads binary data from wire and parses command out of it
     \note  If any timeouts are set on connection, these timers are automatically cancelled
    */
   void
   read_command (
      tcp_connection::pointer                   connection,
      read_command_callback_type const &        callback);

   /*!
     \brief Processes workload via registered callback
    */
   std::string
   process_workload (
      std::string const &       workload);

private:


   /*!
     \brief Callback function for incoming command by conneciton
    */
   void
   handle_command (
      tcp_connection::pointer   connection,
      command const &           command);

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
   workload_processor_callback_type     workload_processor_callback_;

   handshake                            handshake_;
   elect_leader                         elect_leader_;
   announce_leadership                  announce_leadership_;
   basic_paxos                          basic_paxos_;
};

}; }; };

#endif //! LIBPAXOS_CPP_DETAIL_PROTOCOL_PROTOCOL_HPP

