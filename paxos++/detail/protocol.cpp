#include <iostream>
#include <vector>

#include <boost/ref.hpp>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>

#include "../quorum.hpp"
#include "connection_pool.hpp"
#include "protocol.hpp"

namespace paxos { namespace detail {

protocol::protocol (
   boost::asio::io_service &            io_service,
   paxos::detail::connection_pool &     connection_pool,
   paxos::quorum &                      quorum)
   : io_service_ (io_service),
     connection_pool_ (connection_pool),
     quorum_ (quorum)
{
}

void
protocol::bootstrap ()
{
   bootstrap_step1 ();
}


void
protocol::new_connection (
   tcp_connection &     connection)
{

}

void
protocol::bootstrap_step1 ()
{
   std::vector <boost::asio::ip::tcp::endpoint> endpoints;
   quorum_.get_endpoints (std::back_inserter (endpoints));

   BOOST_FOREACH (boost::asio::ip::tcp::endpoint const & endpoint, endpoints)
   {
      tcp_connection & new_connection = connection_pool_.create ();
      
      new_connection.socket ().async_connect (endpoint,
                                              boost::bind (&protocol::bootstrap_step2,
                                                           this,
                                                           boost::ref (new_connection),
                                                           boost::asio::placeholders::error));
   }
}


void
protocol::bootstrap_step2 (
   tcp_connection &                     connection,
   boost::system::error_code const &    error)
{

   std::cout << "established new connection yay!" << std::endl;

}

}; };
