/*!
  Copyright (c) 2012, Leon Mergen, all rights reserved.
 */

#ifndef LIBPAXOS_CPP_DETAIL_CONNECTION_POOL_HPP
#define LIBPAXOS_CPP_DETAIL_CONNECTION_POOL_HPP

#include <stdint.h>
#include <queue>

#include <boost/ref.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>

#include "scoped_connection.hpp"
#include "tcp_connection.hpp"

namespace paxos { namespace detail { namespace connection {

/*!
  \brief Connection pool which maintains tcp_connection objects to remote servers
 */
class pool
{
private:
   friend class scoped_connection;

public:

   pool (
      boost::asio::io_service &                 io_service,
      boost::asio::ip::tcp::endpoint const &    endpoint);

   /*!
     \brief Opens new connection pool
     \param min_spare   Minimum amount of spare connections to hold in pool
     \param max_spare   Maximum amount of spare connections to hold in pool
    */
   void
   open (
      uint16_t  min_spare,
      uint16_t  max_spare);

   /*!
     \brief Checks amount of spare connections and adjusts pool accordingly
    
     This function creates new connections if the amount of spare connections is lower
     than min_spare_, or deletes existing connections if the amount of spare connections
     is more than max_spare_.
    */
   void
   check_spare_connections ();

   /*!
     \brief Returns true if this pool has spare connections
    */
   bool
   has_spare_connections () const;


   /*!
     \brief Returns new scoped_connection
     \pre has_spare_connections () == true
    */
   scoped_connection::pointer
   connection ();

protected:

   /*!
     \brief Used by scoped_connection to acquire connection
    */
   tcp_connection::pointer
   acquire ();


   /*!
     \brief Used by scoped_connection to release connection
    */
   void
   release (
      tcp_connection::pointer);

private:

   void
   connection_established (
      boost::system::error_code const &         error,
      tcp_connection::pointer                   connection);


private:
   boost::asio::io_service &            io_service_;
   boost::asio::ip::tcp::endpoint       endpoint_;

   uint16_t                             min_spare_;
   uint16_t                             max_spare_;

   std::queue <tcp_connection::pointer> connections_;

};

}; }; };

#endif //! LIBPAXOS_CPP_DETAIL_CONNECTION_POOL_HPP
