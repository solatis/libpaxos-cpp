#include <iostream>

#include <boost/ref.hpp>
#include <boost/bind.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/asio/ip/address.hpp>
#include <boost/asio/ip/tcp.hpp>

#include "local_server.hpp"

namespace paxos {

local_server::local_server (
   boost::asio::io_service &            io_service,
   boost::asio::ip::address const &     address,
   uint16_t                             port,
   quorum const &                       quorum)
   : acceptor_ (io_service,
                boost::asio::ip::tcp::endpoint (address, port)),
     quorum_ (quorum),
     connection_pool_ (io_service),
     protocol_ (connection_pool_,
                quorum_)
{
   //! Initialize our unique identification number.
   boost::uuids::basic_random_generator <boost::mt19937> gen;
   uuid_ = gen ();

   //! Make sure the quorum can tell others who we are.
   quorum_.we_are (address, port, uuid_);

   //! Ensure that we start accepting new connections
   accept ();

   //! And bootstrap our protocol by finding out who the current leader is.
   protocol_.bootstrap ();
}


void
local_server::accept ()
{
   detail::tcp_connection & new_connection = connection_pool_.create ();
      detail::tcp_connection::create (acceptor_.get_io_service ());

   acceptor_.async_accept (new_connection.socket (),
                           boost::bind (&local_server::handle_accept,
                                        this,
                                        boost::ref (new_connection),
                                        boost::asio::placeholders::error));
}

void
local_server::close ()
{
   acceptor_.close ();
}


void
local_server::handle_accept (
   detail::tcp_connection &             new_connection,
   boost::system::error_code const &    error)
{
   if (!error)
   {
      protocol_.new_connection (new_connection);
      accept ();
   }
}

};
