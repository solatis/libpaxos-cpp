
#include <paxos++/server.hpp>


int main ()
{
   boost::asio::io_service io_service;

   paxos::server server1 (io_service, "127.0.0.1", 1337);
   paxos::server server2 (io_service, "127.0.0.1", 1338);

   io_service.run ();
   

}
