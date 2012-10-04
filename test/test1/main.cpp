/*!
  This test validates basic paxos operation with a quorum with a single node.
 */


#include <boost/thread/thread.hpp>

#include <paxos++/client.hpp>
#include <paxos++/server.hpp>

int main ()
{
   boost::asio::io_service io_service;
   boost::asio::io_service::work work (io_service); //! Prevents io_service from running out of owrk

   boost::thread io_thread (boost::bind (&boost::asio::io_service::run,
                                         &io_service));

   paxos::configuration::heartbeat_interval = 500;

   paxos::server server1 (io_service,
                          boost::asio::ip::tcp::endpoint (
                             boost::asio::ip::address_v4::from_string ("127.0.0.1"), 1337),
                          [](std::string const &) -> std::string
                          {
                             return "bar";
                          });


   server1.add (boost::asio::ip::tcp::endpoint (
                   boost::asio::ip::address_v4::from_string ("127.0.0.1"), 1337));


   server1.start ();

   paxos::client client (io_service);
   client.add (boost::asio::ip::tcp::endpoint (
                  boost::asio::ip::address_v4::from_string ("127.0.0.1"), 1337));

   client.start ();


   PAXOS_ASSERT (client.send ("foo", 10) == "bar");

   io_service.stop ();
}


