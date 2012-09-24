/*!
  Copyright (c) 2012, Leon Mergen, all rights reserved.
 */

#ifndef LIBPAXOS_CPP_DETAIL_PROTOCOL_HPP
#define LIBPAXOS_CPP_DETAIL_PROTOCOL_HPP

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

namespace paxos { namespace detail {

/*!
  \brief Entry point for communication within quorum
 */
class protocol
{
public:

   /*!
     \brief Constructor
     \param io_service          Our link to the OS'es I/O services.
     \param connection_pool     Used to create new connections
     \param quorum              Quorum of servers we're communicating with.
    */
   protocol (
      boost::asio::io_service &         io_service,
      paxos::detail::connection_pool &  connection_pool,
      paxos::quorum &                   quorum);


   /*!
     \brief Starts finding out who our current leader is, if any.
     
     This opens a new connection to all servers within our quorum and attempts
     to find out who our current leader is, or requests a new election.
    */
   void
   bootstrap ();

   /*!
     \brief Called by local_server when a new connection arrives.
    */
   void
   new_connection (
      tcp_connection &    connection);

private:

   void
   bootstrap_step1 ();

   void
   bootstrap_step2 (
      tcp_connection &                  connection,
      boost::system::error_code const & error);

private:

   boost::asio::io_service &            io_service_;
   paxos::detail::connection_pool &     connection_pool_;
   paxos::quorum &                      quorum_;

};

}; };

#endif //! LIBPAXOS_CPP_DETAIL_TCP_CONNECTION_HPP
