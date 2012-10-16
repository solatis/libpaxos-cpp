/*!
  This test validates whether request pipelining inside the client works properly
 */

#include <atomic>
#include <boost/lexical_cast.hpp>

#include <paxos++/client.hpp>
#include <paxos++/server.hpp>
#include <paxos++/configuration.hpp>
#include <paxos++/detail/util/debug.hpp>

#define TEST_COUNT 1000

int main ()
{
   std::atomic <uint16_t> response_count (0);

   paxos::server::callback_type callback =
      [& response_count](std::string const & workload) -> std::string
      {
         ++response_count;

         return workload;
      };

   paxos::configuration configuration;
   configuration.set_quorum_majority_factor (1.0);

   paxos::server server1 ("127.0.0.1", 1337, callback, configuration);
   paxos::server server2 ("127.0.0.1", 1338, callback, configuration);
   paxos::server server3 ("127.0.0.1", 1339, callback, configuration);

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




   paxos::client client (configuration);
   client.add ("127.0.0.1", 1337);
   client.add ("127.0.0.1", 1338);
   client.add ("127.0.0.1", 1339);
   client.start ();

   std::map <size_t, std::future <std::string> >     results;
   
   for (size_t i = 0; i < TEST_COUNT; ++i)
   {
      results[i] = client.send (boost::lexical_cast <std::string> (i), 10);
   }

   
   for (auto & i : results)
   {
      PAXOS_ASSERT_EQ (i.second.get (), boost::lexical_cast <std::string> (i.first));
   }


   PAXOS_ASSERT_EQ (response_count.load (), 3 * TEST_COUNT);
   PAXOS_INFO ("test succeeded");
}


