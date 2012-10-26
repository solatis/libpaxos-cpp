/*!
  This test validates that no progress is made when there are too few servers online.
 */

#include <paxos++/client.hpp>
#include <paxos++/server.hpp>
#include <paxos++/exception/exception.hpp>
#include <paxos++/detail/util/debug.hpp>

int main ()
{
   paxos::server::callback_type callback = 
      [](int64_t, std::string const &) -> std::string
      {
         return "bar";
      };

   paxos::server server1 ("127.0.0.1", 1337, callback);
   server1.add ({{"127.0.0.1", 1337}, {"127.0.0.1", 1338}, {"127.0.0.1", 1339}});

   paxos::client client;
   client.add  ({{"127.0.0.1", 1337}, {"127.0.0.1", 1338}, {"127.0.0.1", 1339}});

   std::future <std::string> future = client.send ("foo");

   PAXOS_ASSERT_THROW (future.get (), paxos::exception::no_majority);

   /*!
     Validate the whole thing recovers as soon as the new servers are online.
    */

   paxos::server server2 ("127.0.0.1", 1338, callback);
   paxos::server server3 ("127.0.0.1", 1339, callback);
   server2.add ({{"127.0.0.1", 1337}, {"127.0.0.1", 1338}, {"127.0.0.1", 1339}});
   server3.add ({{"127.0.0.1", 1337}, {"127.0.0.1", 1338}, {"127.0.0.1", 1339}});


   PAXOS_ASSERT_EQ (client.send ("foo").get (), "bar");


   PAXOS_INFO ("test succeeded");


}


