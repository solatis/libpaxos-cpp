/*!
  Copyright (c) 2012, Leon Mergen, all rights reserved.
 */

#ifndef LIBPAXOS_CPP_DETAIL_PROTOCOL_PROTOCOL_HPP
#define LIBPAXOS_CPP_DETAIL_PROTOCOL_PROTOCOL_HPP

#include <boost/bind.hpp>

#include "../util/conversion.hpp"
#include "pb/adapter.hpp"
#include "pb/command.pb.h"
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

/*!
  \brief Entry point for communication within quorum
 */
class protocol
{
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
     \brief Serializes a protocolbuffers command to a string and sends it over the wire
    */
   static void
   write_command (
      pb::command const &       command,
      tcp_connection &          output);

   /*!
     \brief Reads binary data from wire and parses command out of it
    */
   template <typename Callback>
   static void
   read_command (
      tcp_connection &  input,
      Callback          output);

private:

   template <typename Iterator>
   static std::pair <Iterator, bool>
   match_command (
      Iterator,
      Iterator);


   template <typename Callback>
   static void
   parse_command (
      tcp_connection &                  input,
      boost::system::error_code const & error,
      size_t                            bytes_transferred,
      Callback                          callback);

   /*!
     \brief Reads byte array of command from stream
     \returns Returns true if succesful, false otherwise
    */
   template <typename Iterator>
   static bool
   read_command_byte_array (
      Iterator          begin,
      Iterator          end,
      std::string &     output);


private:

   paxos::detail::connection_pool &     connection_pool_;
   paxos::quorum &                      quorum_;

   session <elect_leader>               elect_leader_;
};

}; }; };

#include "protocol.inl"

#endif //! LIBPAXOS_CPP_DETAIL_PROTOCOL_PROTOCOL_HPP

