/*!
  Copyright (c) 2012, Leon Mergen, all rights reserved.
 */

#ifndef LIBPAXOS_CPP_DETAIL_TCP_CONNECTION_HPP
#define LIBPAXOS_CPP_DETAIL_TCP_CONNECTION_HPP

#include <vector>

#include <boost/asio/ip/tcp.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/thread/mutex.hpp>

#include "command_dispatcher.hpp"
#include "tcp_connection_fwd.hpp"

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

   ~tcp_connection ();

   static tcp_connection_ptr
   create (
      boost::asio::io_service &        io_service);   


   boost::asio::ip::tcp::socket &
   socket ();

   void
   close ();

   void
   write (
      std::string const &       message);

   /*!  
     \brief Access to the underlying command dispatcher
    */
   detail::command_dispatcher &
   command_dispatcher ();

private:


   tcp_connection (
      boost::asio::io_service & io_service);

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

   detail::command_dispatcher   command_dispatcher_;
};

}; };

#endif //! LIBPAXOS_CPP_DETAIL_TCP_CONNECTION_HPP
