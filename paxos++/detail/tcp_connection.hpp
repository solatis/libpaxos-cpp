/*!
  Copyright (c) 2012, Leon Mergen, all rights reserved.
 */

#ifndef LIBPAXOS_CPP_DETAIL_TCP_CONNECTION_HPP
#define LIBPAXOS_CPP_DETAIL_TCP_CONNECTION_HPP

#include <vector>

#include <boost/function.hpp>
#include <boost/optional.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/thread/mutex.hpp>

#include "error.hpp"
#include "tcp_connection_fwd.hpp"

namespace paxos { 
class configuration;
};

namespace paxos { namespace detail {
class command;
class parser;
}; };

namespace paxos { namespace detail { namespace quorum {
class quorum;
}; }; };

namespace paxos { namespace detail { 

/*!
  \brief Represents a tcp connection

  This is a plain wrapper around a TCP socket.

  Note that this does not necessarily needs to represent a long-lived connection to another 
  server inside the quorum: since the quorum can also build up short-lived connections to each
  other, and clients can also connect to the local server, this can be a connection to anything.
 */
class tcp_connection 
   : public boost::enable_shared_from_this <tcp_connection>
{
public:

   friend detail::parser;

   typedef boost::function <void (boost::optional <enum error_code>,
                                  command const &)>                     read_callback;

public:
   ~tcp_connection ();

   static tcp_connection_ptr
   create (
      boost::asio::io_service &                 io_service);


   boost::asio::ip::tcp::socket &
   socket ();

   /*!
     \brief Closes socket ()
    */
   void
   close ();

   /*!
     \brief Returns true if socket () is open
    */
   bool
   is_open () const;

   /*!
     \brief Writes a command to the other side
    */
   void
   write_command (
      detail::command const &   command);

   /*!
     \brief Reads a command from the other side
    */
   void
   read_command (
      read_callback             callback);

   /*!
     \brief Keeps reading commands  until connection error occurs

     This function is used by each paxos::server to respond to commands from accepted
     connections. In essense, this is used by the leader to read input from all clients,
     and used by all followers to read input from the leader.
    */
   void
   read_command_loop (
      read_callback             callback);

private:

   tcp_connection (
      boost::asio::io_service &                 io_service);

   void
   write (
      std::string const &       message);

   void
   write_locked (
      std::string const &       message);

   void
   start_write_locked ();

   void
   handle_write (
      boost::system::error_code const & error,
      size_t                            bytes_transferred);

   void
   handle_write_locked (
      size_t                            bytes_transferred);

private:

   boost::asio::ip::tcp::socket socket_;

   /*!
     \brief Synchronizes access to write_buffer_
    */
   boost::mutex                 mutex_;

   std::string                  write_buffer_;
};

}; };

#endif //! LIBPAXOS_CPP_DETAIL_TCP_CONNECTION_HPP
