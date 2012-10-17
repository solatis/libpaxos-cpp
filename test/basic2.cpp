/*!
  Same basic test as test1, except this time with a quorum of 3 servers instead of 1.
 */


#include <paxos++/client.hpp>
#include <paxos++/server.hpp>
#include <paxos++/detail/util/debug.hpp>

int main ()
{
   uint16_t response_count = 0;

   paxos::server::callback_type callback = 
      [& response_count](std::string const &) -> std::string
      {
         ++response_count;
         return "bar";
      };

   paxos::server server1 ("127.0.0.1", 1337, callback);
   paxos::server server2 ("127.0.0.1", 1338, callback);
   paxos::server server3 ("127.0.0.1", 1339, callback);

   server1.add ("127.0.0.1", 1337);
   server1.add ("127.0.0.1", 1338);
   server1.add ("127.0.0.1", 1339);

   server2.add ("127.0.0.1", 1337);
   server2.add ("127.0.0.1", 1338);
   server2.add ("127.0.0.1", 1339);

   server3.add ("127.0.0.1", 1337);
   server3.add ("127.0.0.1", 1338);
   server3.add ("127.0.0.1", 1339);

   server1.start ();
   server2.start ();
   server3.start ();

   paxos::client client;
   client.add ("127.0.0.1", 1337);
   client.add ("127.0.0.1", 1338);
   client.add ("127.0.0.1", 1339);
   client.start ();
   client.wait_until_quorum_ready ();


   PAXOS_ASSERT_EQ (client.send ("foo").get (), "bar");
   PAXOS_ASSERT_EQ (response_count, 3);


   PAXOS_INFO ("test succeeded");
}


