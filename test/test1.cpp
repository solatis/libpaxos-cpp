
#include <paxos++/local_server.hpp>


int main ()
{
   boost::asio::io_service io_service;


   paxos::quorum quorum;
   quorum.add (boost::asio::ip::address_v4::from_string ("127.0.0.1"), 1337);
   quorum.add (boost::asio::ip::address_v4::from_string ("127.0.0.1"), 1338);
   quorum.add (boost::asio::ip::address_v4::from_string ("127.0.0.1"), 1339);

   paxos::local_server server1 (io_service, 
                                boost::asio::ip::address_v4::from_string ("127.0.0.1"), 1337,
                                quorum);

   paxos::local_server server2 (io_service, 
                                boost::asio::ip::address_v4::from_string ("127.0.0.1"), 1338,
                                quorum);

   paxos::local_server server3 (io_service, 
                                boost::asio::ip::address_v4::from_string ("127.0.0.1"), 1339,
                                quorum);

   io_service.run ();
   

}
