/*!
  Tests whether the quorum will properly handle a dead node and it coming alive in a later stadium
 */

#include <boost/date_time/posix_time/posix_time_duration.hpp>

#include <paxos++/client.hpp>
#include <paxos++/server.hpp>
#include <paxos++/configuration.hpp>
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

   server1.add ("127.0.0.1", 1337);
   server1.add ("127.0.0.1", 1338);
   server1.add ("127.0.0.1", 1339);

   server2.add ("127.0.0.1", 1337);
   server2.add ("127.0.0.1", 1338);
   server2.add ("127.0.0.1", 1339);

   paxos::client client;
   client.add ("127.0.0.1", 1337);
   client.add ("127.0.0.1", 1338);
   client.add ("127.0.0.1", 1339);


   PAXOS_ASSERT_EQ (client.send ("foo").get (), "bar");
   PAXOS_ASSERT_EQ (client.send ("foo").get (), "bar");
   PAXOS_ASSERT_EQ (client.send ("foo").get (), "bar");
   PAXOS_ASSERT_EQ (client.send ("foo").get (), "bar");
   PAXOS_ASSERT_EQ (client.send ("foo").get (), "bar");
   PAXOS_ASSERT_EQ (client.send ("foo").get (), "bar");
   PAXOS_ASSERT_EQ (client.send ("foo").get (), "bar");
   PAXOS_DEBUG ("response_count = " << response_count);

   PAXOS_ASSERT_EQ (response_count, 14);
   response_count = 0;

   paxos::server server3 ("127.0.0.1", 1339, callback);

   server3.add ("127.0.0.1", 1337);
   server3.add ("127.0.0.1", 1338);
   server3.add ("127.0.0.1", 1339);


   /*!
     Let's wait a few seconds for 3 handshakes to occur
    */
   boost::this_thread::sleep (
      boost::posix_time::milliseconds (
         paxos::configuration ().heartbeat_interval ()));


   do
   {
      response_count = 0;
      PAXOS_ASSERT_EQ (client.send ("foo").get (), "bar");   
   } while (response_count < 3);

   PAXOS_INFO ("test succeeded");
}


