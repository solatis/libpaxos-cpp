/*!
  Copyright (c) 2012, Leon Mergen, all rights reserved.
 */

#ifndef LIBPAXOS_CPP_DETAIL_TCP_CONNECTION_HPP
#define LIBPAXOS_CPP_DETAIL_TCP_CONNECTION_HPP

#include <vector>

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

namespace paxos { namespace detail { namespace connection {

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

   typedef boost::shared_ptr <tcp_connection>                                   pointer;

private:

   friend class tcp_connection_usage_guard;

public:

   ~tcp_connection ();

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

   /*!
     \brief Starts timer that calls socket_.cancel () when done

     This is useful to issue right before an async_read is done on a connection when
     a response is expected. Due to it calling socket_.cancel () after the timeout has
     occurred, the async_read () will return with a boost::asio::error::operation_aborted
     error.
     
     The caller must ensure that cancel_timeout () is called after a response has actually
     been received.
    */
   void
   start_timeout (
      boost::asio::deadline_timer::duration_type const &        expiry_time);

   void
   cancel_timeout ();


private:

   tcp_connection (
      boost::asio::io_service & io_service);

   void
   start_write ();

   void
   handle_write (
      boost::system::error_code const & error,
      size_t                            bytes_transferred);

   void
   handle_timeout (
      boost::system::error_code const & error);

private:
   boost::asio::ip::tcp::socket socket_;
   boost::asio::deadline_timer  timeout_;

   std::string                  write_buffer_;
};

}; }; };

#endif //! LIBPAXOS_CPP_DETAIL_TCP_CONNECTION_HPP
