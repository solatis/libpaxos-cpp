#include <iostream>

#include <boost/bind.hpp>

#include "server.hpp"

namespace paxos {

server::server (
   boost::asio::io_service &    io_service,
   std::string const &          address,
   int16_t                      port)
   : acceptor_ (io_service,
                boost::asio::ip::tcp::endpoint (boost::asio::ip::address_v4::from_string (address),
                                                port))
{
   accept ();
}


void
server::accept ()
{
   detail::tcp_connection::pointer new_connection =
      detail::tcp_connection::create (acceptor_.io_service ());

   acceptor_.async_accept (new_connection->socket (),
                           boost::bind (&server::handle_accept,
                                        this,
                                        new_connection,
                                        boost::asio::placeholders::error));
}

void
server::close ()
{
   acceptor_.close ();
}


void
server::handle_accept (
   detail::tcp_connection::pointer      new_connection,
   boost::system::error_code const &    error)
{
   if (!error)
   {
      accept ();
   }
}

};
