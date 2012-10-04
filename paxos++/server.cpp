#include <iostream>

#include <boost/ref.hpp>
#include <boost/bind.hpp>
#include <boost/asio/placeholders.hpp>
#include <boost/asio/ip/address.hpp>
#include <boost/asio/ip/tcp.hpp>

#include "exception/exception.hpp"

#include "detail/util/debug.hpp"
#include "detail/dispatcher.hpp"
#include "detail/parser.hpp"
#include "server.hpp"

namespace paxos {


server::server (
   std::string const &                          host,
   uint16_t                                     port,
   detail::paxos_state::processor_type const &  processor)
   : server (io_thread_.io_service (),
             host,
             port,
             processor)
{
   io_thread_.launch ();
}


server::server (
   boost::asio::io_service &                    io_service,
   std::string const &                          host,
   uint16_t                                     port,
   detail::paxos_state::processor_type const &  processor)
   : acceptor_ (io_service,
                boost::asio::ip::tcp::endpoint (
                   boost::asio::ip::address::from_string (host), port)),
     quorum_ (io_service,
                boost::asio::ip::tcp::endpoint (
                   boost::asio::ip::address::from_string (host), port)),
     state_ (processor)
{
   /*! 
     Ensure that we start accepting new connections. We do this before
     start (), so that we can first construct multiple servers at the same time,
     and then when they are started, all servers are already accepting connections.
   */
   accept ();
}

server::~server ()
{
   close ();
}

void
server::start ()
{
   quorum_.bootstrap ();
}


void
server::close ()
{
   acceptor_.close ();
   io_thread_.stop ();
}


void
server::add (
   std::string const &  host,
   uint16_t             port)
{
   quorum_.add (
      boost::asio::ip::tcp::endpoint (
         boost::asio::ip::address::from_string (host), port));
}


void
server::accept ()
{
   detail::tcp_connection::pointer connection = 
      detail::tcp_connection::create (acceptor_.get_io_service ());

   acceptor_.async_accept (connection->socket (),
                           boost::bind (&server::handle_accept,
                                        this,
                                        connection,
                                        boost::asio::placeholders::error));
}


void
server::handle_accept (
   detail::tcp_connection::pointer      new_connection,
   boost::system::error_code const &    error)
{
   if (error)
   {
      PAXOS_ERROR ("Unable to accept connection: " << error.message ());
      return;
   }
   
   server::read_and_dispatch_command (new_connection,
                                      quorum_,
                                      state_);
   
   /*!
     Enter "recursion" by accepting a new connection
   */
   accept ();
}

/*! static */ void
server::read_and_dispatch_command (
   detail::tcp_connection::pointer      connection,
   detail::quorum::quorum &             quorum,
   detail::paxos_state &                state)
{
   /*!
     The code below essentially creates an infinite "recusion" loop, without
     the recursion (since the async_reads in the parser unwind the stack).

     \warning This also means that no other async_read can occur on this
              connection, and that this loop "owns" this connection. 

     \todo We could enforce this by making a separate "writable" and "readable"
           interface for the connection; if we could somehow give the dispatcher
           only a reference to the "writable" connection and not the "readable"
           connection, this could be enforced by design.
    */
   detail::parser::read_command (connection,
                                 [connection,
                                  & quorum,
                                  & state] (detail::command const & command)
                                 {
                                    detail::dispatcher::dispatch_command (connection,
                                                                          command,
                                                                          quorum,
                                                                          state);

                                    //! Enters recursion
                                    server::read_and_dispatch_command (connection,
                                                                       quorum,
                                                                       state);
                                 });

};

};
