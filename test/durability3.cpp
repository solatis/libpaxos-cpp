/*!
  Ensures that logs are cleaned periodically so we do not have eternal logs in history,
  but logs are not cleaned when a node is down.
 */

#include <boost/thread/mutex.hpp>

#include <paxos++/client.hpp>
#include <paxos++/server.hpp>
#include <paxos++/durable/storage.hpp>
#include <paxos++/exception/exception.hpp>
#include <paxos++/detail/util/debug.hpp>

bool
all_responses_equal (
   std::map <int64_t, uint16_t> const & responses,
   uint16_t                             count)
{
   for (auto const & i : responses)
   {
      if (i.second != count)
      {
         return false;
      }
   }

   return true;
}

int main ()
{
   paxos::configuration configuration1;
   paxos::configuration configuration2;
   paxos::configuration configuration3;

   configuration1.durable_storage ().set_history_size (5);
   configuration2.durable_storage ().set_history_size (5);
   configuration3.durable_storage ().set_history_size (5);

   paxos::server::callback_type callback = 
      [](
         int64_t                promise_id,
         std::string const &    workload) -> std::string
      {
         return "bar";
      };

   paxos::client client;
   client.add  ({{"127.0.0.1", 1337}, {"127.0.0.1", 1338}, {"127.0.0.1", 1339}});

   paxos::server server1 ("127.0.0.1", 1337, callback, configuration1);
   paxos::server server2 ("127.0.0.1", 1338, callback, configuration2);
   
   server1.add ({{"127.0.0.1", 1337}, {"127.0.0.1", 1338}, {"127.0.0.1", 1339}});
   server2.add ({{"127.0.0.1", 1337}, {"127.0.0.1", 1338}, {"127.0.0.1", 1339}});

   {
      paxos::server server3 ("127.0.0.1", 1339, callback, configuration3);
      server3.add ({{"127.0.0.1", 1337}, {"127.0.0.1", 1338}, {"127.0.0.1", 1339}});
    
      /*!
        Issue 10 calls, so we can verify that the lowest proposal id is in fact 10.
       */
      for (size_t i = 0; i < 15; ++i)
      {
         PAXOS_ASSERT_EQ (client.send ("foo").get (), "bar");
      }

      PAXOS_ASSERT_EQ (configuration1.durable_storage ().lowest_proposal_id (), 11);
      PAXOS_ASSERT_EQ (configuration2.durable_storage ().lowest_proposal_id (), 11);
      PAXOS_ASSERT_EQ (configuration3.durable_storage ().lowest_proposal_id (), 11);
   }
   
   /*!
     Now, since one host in our quorum is currently down, logs should *not* be cleaned
     up further than the proposal that has been previously accepted by that host.
    */

   for (size_t i = 0; i < 30; ++i)
   {
      PAXOS_ASSERT_EQ (client.send ("foo").get (), "bar");
   }

   PAXOS_ASSERT_GE (configuration1.durable_storage ().lowest_proposal_id (), 15);
   PAXOS_ASSERT_GE (configuration2.durable_storage ().lowest_proposal_id (), 15);
   PAXOS_ASSERT_EQ (configuration3.durable_storage ().lowest_proposal_id (), 11);

   boost::this_thread::sleep (
      boost::posix_time::milliseconds (
         paxos::configuration ().timeout ()));

   {
      /*!
        And after our server is back online, the history can be cleared again.
       */

      paxos::server server3 ("127.0.0.1", 1339, callback, configuration3);
      server3.add ({{"127.0.0.1", 1337}, {"127.0.0.1", 1338}, {"127.0.0.1", 1339}});
    
      for (size_t i = 0; i < 15; ++i)
      {
         PAXOS_ASSERT_EQ (client.send ("foo").get (), "bar");
      }

      PAXOS_ASSERT_EQ (configuration1.durable_storage ().lowest_proposal_id (), 56);
      PAXOS_ASSERT_EQ (configuration2.durable_storage ().lowest_proposal_id (), 56);
      PAXOS_ASSERT_EQ (configuration3.durable_storage ().lowest_proposal_id (), 56);
   }


   
   PAXOS_INFO ("test succeeded");
}
