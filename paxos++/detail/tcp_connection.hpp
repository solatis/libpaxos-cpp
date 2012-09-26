/*!
  Copyright (c) 2012, Leon Mergen, all rights reserved.
 */

#ifndef LIBPAXOS_CPP_DETAIL_TCP_CONNECTION_HPP
#define LIBPAXOS_CPP_DETAIL_TCP_CONNECTION_HPP

#include <vector>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "util/debug.hpp"

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

   typedef boost::shared_ptr <tcp_connection>   pointer;

   static pointer 
   create (
      boost::asio::io_service &        io_service);

   void
   close ();

   boost::asio::ip::tcp::socket &
   socket ();

   void
   write (
      std::string const &       message);   

private:

   tcp_connection (
      boost::asio::io_service & io_service);

   void
   start_write ();

   void
   handle_write (
      boost::system::error_code const & error,
      size_t                            bytes_transferred);

private:

   boost::asio::ip::tcp::socket socket_;

   std::string                  write_buffer_;
};

}; };

#endif //! LIBPAXOS_CPP_DETAIL_TCP_CONNECTION_HPP
