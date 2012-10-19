/*!
  This test validates basic paxos operation with a quorum with a single node.
 */


#include <paxos++/client.hpp>
#include <paxos++/server.hpp>
#include <paxos++/detail/util/debug.hpp>

int main ()
{
   paxos::server server ("127.0.0.1", 1337,
                         [](std::string const &) -> std::string
                         {
                            return "bar";
                         });
   server.add ("127.0.0.1", 1337);

   paxos::client client;
   client.add ("127.0.0.1", 1337);

   std::future <std::string> future = client.send ("foo");
   PAXOS_ASSERT_EQ (future.get (), "bar");

   PAXOS_INFO ("test succeeded");
}


