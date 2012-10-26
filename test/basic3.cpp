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
      [& response_count](int64_t, std::string const & workload) -> std::string
      {
         ++response_count;

         return workload;
      };

   paxos::server server1 ("127.0.0.1", 1337, callback);
   paxos::server server2 ("127.0.0.1", 1338, callback);
   paxos::server server3 ("127.0.0.1", 1339, callback);
   paxos::client client;

   server1.add ({{"127.0.0.1", 1337}, {"127.0.0.1", 1338}, {"127.0.0.1", 1339}});
   server2.add ({{"127.0.0.1", 1337}, {"127.0.0.1", 1338}, {"127.0.0.1", 1339}});
   server3.add ({{"127.0.0.1", 1337}, {"127.0.0.1", 1338}, {"127.0.0.1", 1339}});
   client.add  ({{"127.0.0.1", 1337}, {"127.0.0.1", 1338}, {"127.0.0.1", 1339}});

   std::map <size_t, std::future <std::string> >     results;
   
   for (size_t i = 0; i < TEST_COUNT; ++i)
   {
      results[i] = client.send (boost::lexical_cast <std::string> (i));
   }

   
   for (auto & i : results)
   {
      PAXOS_DEBUG ("validating result " << i.first);
      PAXOS_ASSERT_EQ (i.second.get (), boost::lexical_cast <std::string> (i.first));
   }


   PAXOS_ASSERT_EQ (response_count.load (), 3 * TEST_COUNT);
   PAXOS_INFO ("test succeeded");
}


