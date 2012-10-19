#include <iostream>
#include <functional>

#include <boost/asio/placeholders.hpp>
#include <boost/asio/ip/address.hpp>
#include <boost/asio/ip/tcp.hpp>

#include "exception/exception.hpp"

#include "detail/util/debug.hpp"
#include "detail/parser.hpp"
#include "detail/tcp_connection.hpp"
#include "detail/command_dispatcher.hpp"
#include "server.hpp"

namespace paxos {


server::server (
   std::string const &                  host,
   uint16_t                             port,
   callback_type const &                processor,
   paxos::configuration                 configuration)
   : server (io_thread_.io_service (),
             host,
             port,
             processor,
             configuration)
{
   io_thread_.launch ();
}

server::server (
   boost::asio::io_service &            io_service,
   std::string const &                  host,
   uint16_t                             port,
   callback_type const &                processor,
   paxos::configuration                 configuration)
   : acceptor_ (io_service,
                boost::asio::ip::tcp::endpoint (
                   boost::asio::ip::address::from_string (host), port)),

     quorum_ (io_service,
              boost::asio::ip::tcp::endpoint (
                 boost::asio::ip::address::from_string (host), port),
              configuration),

     state_ (processor,
             configuration)
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
   stop ();
}


void
server::stop ()
{
   acceptor_.close ();
   io_thread_.stop ();
}


void
server::add (
   std::initializer_list <std::pair <std::string, uint16_t> > const &        servers)
{
   for (auto const & i : servers)
   {
      this->add (i.first,
                 i.second);
   }
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
   detail::tcp_connection_ptr connection = 
      detail::tcp_connection::create (acceptor_.get_io_service ());

   acceptor_.async_accept (connection->socket (),
                           std::bind (&server::handle_accept,
                                      this,
                                      connection,
                                      std::placeholders::_1));
}


void
server::handle_accept (
   detail::tcp_connection_ptr           new_connection,
   boost::system::error_code const &    error)
{
   if (error)
   {
      PAXOS_ERROR ("Unable to accept connection: " << error.message ());
      return;
   }

   new_connection->read_command_loop (
      std::bind (&detail::command_dispatcher::dispatch_command,
                 std::placeholders::_1,
                 new_connection,
                 std::placeholders::_2,
                 std::ref (quorum_),
                 std::ref (state_)));
   /*!
     Enter "recursion" by accepting a new connection
   */
   accept ();
}

};
