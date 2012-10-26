/*!
  Tests whether the quorum will properly handle a dead node and it coming alive in a later stadium,
  including a proper catch up.
 */

#include <boost/date_time/posix_time/posix_time_duration.hpp>

#include <paxos++/client.hpp>
#include <paxos++/server.hpp>
#include <paxos++/configuration.hpp>
#include <paxos++/detail/util/debug.hpp>

int main ()
{
   uint16_t calls = 0;
   uint16_t response_count = 0;
   paxos::server::callback_type callback = 
      [& response_count](std::string const &) -> std::string
      {
         ++response_count;
         return "bar";
      };

   paxos::server server1 ("127.0.0.1", 1337, callback);
   paxos::server server2 ("127.0.0.1", 1338, callback);
   paxos::client client;


   server1.add ({{"127.0.0.1", 1337}, {"127.0.0.1", 1338}, {"127.0.0.1", 1339}});
   server2.add ({{"127.0.0.1", 1337}, {"127.0.0.1", 1338}, {"127.0.0.1", 1339}});
   client.add  ({{"127.0.0.1", 1337}, {"127.0.0.1", 1338}, {"127.0.0.1", 1339}});

   PAXOS_ASSERT_EQ (client.send ("foo").get (), "bar");
   PAXOS_ASSERT_EQ (client.send ("foo").get (), "bar");
   PAXOS_ASSERT_EQ (client.send ("foo").get (), "bar");
   PAXOS_ASSERT_EQ (client.send ("foo").get (), "bar");
   PAXOS_ASSERT_EQ (client.send ("foo").get (), "bar");
   PAXOS_ASSERT_EQ (client.send ("foo").get (), "bar");
   PAXOS_ASSERT_EQ (client.send ("foo").get (), "bar");
   PAXOS_DEBUG ("response_count = " << response_count);
   calls = 7;

   PAXOS_ASSERT_EQ (response_count, 2 * calls);

   paxos::server server3 ("127.0.0.1", 1339, callback);
   server3.add ({{"127.0.0.1", 1337}, {"127.0.0.1", 1338}, {"127.0.0.1", 1339}});

   /*!
     Let's wait a few seconds for 3 handshakes to occur
    */
   boost::this_thread::sleep (
      boost::posix_time::milliseconds (
         paxos::configuration ().timeout ()));

   do
   {
      PAXOS_ASSERT_EQ (client.send ("foo").get (), "bar");   
      ++calls;
      
   } while (response_count != 3 * calls && calls < 50);

   PAXOS_ASSERT_EQ (response_count, 3 * calls);

   PAXOS_INFO ("test succeeded");
}


