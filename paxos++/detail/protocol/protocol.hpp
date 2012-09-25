/*!
  Copyright (c) 2012, Leon Mergen, all rights reserved.
 */

#ifndef LIBPAXOS_CPP_DETAIL_PROTOCOL_PROTOCOL_HPP
#define LIBPAXOS_CPP_DETAIL_PROTOCOL_PROTOCOL_HPP

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

   paxos::detail::connection_pool &
   connection_pool ();

   paxos::quorum &
   quorum ();
   

private:

private:

   paxos::detail::connection_pool &     connection_pool_;
   paxos::quorum &                      quorum_;

   session <elect_leader>               elect_leader_;
};

}; }; };

#endif //! LIBPAXOS_CPP_DETAIL_PROTOCOL_PROTOCOL_HPP
