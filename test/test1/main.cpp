/*!
  This test validates basic paxos operation with a quorum with a single node.
 */


#include <paxos++/client.hpp>
#include <paxos++/server.hpp>

int main ()
{
   paxos::server server ("127.0.0.1", 1337,
                         [](std::string const &) -> std::string
                         {
                            return "bar";
                         });
   server.add ("127.0.0.1", 1337);
   server.start ();




   paxos::client client;
   client.add ("127.0.0.1", 1337);
   client.start ();


   PAXOS_ASSERT (client.send ("foo", 10) == "bar");
}


