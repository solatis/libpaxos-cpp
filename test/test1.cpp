
#include <iostream>
#include <boost/thread/thread.hpp>

#include <paxos++/client.hpp>
#include <paxos++/server.hpp>
#include <paxos++/quorum.hpp>

int main ()
{
   boost::asio::io_service io_service;
   boost::asio::io_service::work work (io_service); //1 Prevents io_service from running out of owrk

   boost::thread io_thread (boost::bind (&boost::asio::io_service::run,
                                         &io_service));

   paxos::quorum quorum;
   quorum.add (boost::asio::ip::address_v4::from_string ("127.0.0.1"), 1337);
   quorum.add (boost::asio::ip::address_v4::from_string ("127.0.0.1"), 1338);
   quorum.add (boost::asio::ip::address_v4::from_string ("127.0.0.1"), 1339);

   paxos::server server1 (io_service,
                          boost::asio::ip::address_v4::from_string ("127.0.0.1"), 1337,
                          quorum);         
   paxos::server server2 (io_service,
                          boost::asio::ip::address_v4::from_string ("127.0.0.1"), 1338,
                          quorum);         
   paxos::server server3 (io_service,
                          boost::asio::ip::address_v4::from_string ("127.0.0.1"), 1339,
                          quorum);         

   paxos::client client1 (io_service,
                          quorum);


   PAXOS_ASSERT (client1.send ("foo", 10) == "bar");

   io_service.stop ();
}


