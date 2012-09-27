/*!
  Copyright (c) 2012, Leon Mergen, all rights reserved.
 */

#ifndef LIBPAXOS_CPP_DETAIL_PROTOCOL_PROTOCOL_HPP
#define LIBPAXOS_CPP_DETAIL_PROTOCOL_PROTOCOL_HPP

#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>

#include "session.hpp"
#include "elect_leader.hpp"

namespace boost { namespace asio {
class io_service;
}; };

namespace paxos { 
class quorum;
};

namespace paxos { namespace detail {
class connection_pool;
class tcp_connection;
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
      paxos::detail::connection_pool &  connection_pool,
      paxos::quorum &                   quorum);


   /*!
     \brief Access to the underlying connection pool
    */
   paxos::detail::connection_pool &
   connection_pool ();

   /*!
     \brief Access to the underlying quorum
    */
   paxos::quorum &
   quorum ();

   /*!
     \brief Starts leader election
    */
   void
   bootstrap ();

   /*!
     \brief Called by local_server when a new connection arrives.
    */
   void
   new_connection (
      tcp_connection &    connection);

   /*!
     \brief Callback function for incoming command by conneciton
    */
   void
   handle_command (
      tcp_connection &  connection,
      command const &   command);

   /*!
     \brief Serializes a protocolbuffers command to a string and sends it over the wire
    */
   static void
   write_command (
      command const &   command,
      tcp_connection &  output);

   /*!
     \brief Reads binary data from wire and parses command out of it
    */
   static void
   read_command (
      tcp_connection &                          connection,
      read_command_callback_type const &        callback);

private:

   static void
   read_command_parse_size (
      tcp_connection &                                  connection,
      boost::system::error_code const &                 error,
      size_t                                            bytes_transferred,
      boost::shared_array <char>                        buffer,
      boost::shared_ptr <read_command_callback_type>    callback);


   static void
   read_command_parse_command (
      tcp_connection &                                  connection,
      boost::system::error_code const &                 error,
      size_t                                            bytes_transferred,
      boost::shared_array <char>                        buffer,
      boost::shared_ptr <read_command_callback_type>    callback);

private:

   paxos::detail::connection_pool &     connection_pool_;
   paxos::quorum &                      quorum_;

   session <elect_leader>               elect_leader_;
};

}; }; };

#endif //! LIBPAXOS_CPP_DETAIL_PROTOCOL_PROTOCOL_HPP

