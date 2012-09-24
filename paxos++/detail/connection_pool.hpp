/*!
  Copyright (c) 2012, Leon Mergen, all rights reserved.
 */

#ifndef LIBPAXOS_CPP_DETAIL_CONNECTION_POOL_HPP
#define LIBPAXOS_CPP_DETAIL_CONNECTION_POOL_HPP

#include <vector>
#include <boost/asio.hpp>

#include "tcp_connection.hpp"

namespace paxos { namespace detail {

/*!
  \brief Maintains a pool of tcp_connection objects.

  The function of this pool is to ensure the tcp_connection's have an actual owner, never
  go out of scope, and can be periodically cleaned for idle connections.
 */
class connection_pool
{
public:

   connection_pool (
      boost::asio::io_service & io_service);

   /*!
     \brief Creates a new connection and associated the endpoint with it
    */
   tcp_connection &
   create ();

private:

   boost::asio::io_service &                    io_service_;
   std::vector <tcp_connection::pointer>        connections_;
   
};

}; };

#endif //! LIBPAXOS_CPP_DETAIL_TCP_CONNECTION_HPP
