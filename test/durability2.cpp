/*!
  Starts to query the quorum, slowly shutting down all servers, restarting them, and verifying
  that a catch-up works.
 */

#include <boost/thread/mutex.hpp>

#include <paxos++/client.hpp>
#include <paxos++/server.hpp>
#include <paxos++/durable/heap.hpp>
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
   std::map <int64_t, uint16_t> responses;

   /*!
     Synchronizes access to responses
    */
   boost::mutex mutex;

   paxos::configuration configuration1;
   paxos::configuration configuration2;
   paxos::configuration configuration3;

   /*!
     Note that the configuration objects below outlive the server objects declared later in
     the test, thus providing semi-durable storage.
    */
   configuration1.set_durable_storage (
      new paxos::durable::heap ());
   configuration2.set_durable_storage (
      new paxos::durable::heap ());
   configuration3.set_durable_storage (
      new paxos::durable::heap ());

   paxos::server::callback_type callback = 
      [& responses,
       & mutex](
         int64_t                promise_id,
         std::string const &    workload) -> std::string
      {
         boost::mutex::scoped_lock lock (mutex);

         if (responses.find (promise_id) == responses.end ())
         {
            responses[promise_id] = 1;
         }
         else
         {
            responses[promise_id]++;
         }

         PAXOS_ASSERT (responses[promise_id] <= 3);

         return "bar";
      };

   paxos::client client;
   client.add  ({{"127.0.0.1", 1337}, {"127.0.0.1", 1338}, {"127.0.0.1", 1339}});

   {
      paxos::server server1 ("127.0.0.1", 1337, callback, configuration1);

      server1.add ({{"127.0.0.1", 1337}, {"127.0.0.1", 1338}, {"127.0.0.1", 1339}});

      {
         paxos::server server2 ("127.0.0.1", 1338, callback, configuration2);

         server2.add ({{"127.0.0.1", 1337}, {"127.0.0.1", 1338}, {"127.0.0.1", 1339}});

         {
            paxos::server server3 ("127.0.0.1", 1339, callback, configuration3);

            server3.add ({{"127.0.0.1", 1337}, {"127.0.0.1", 1338}, {"127.0.0.1", 1339}});

            PAXOS_ASSERT_EQ (client.send ("foo").get (), "bar");
            PAXOS_ASSERT_EQ (client.send ("foo").get (), "bar");
            PAXOS_ASSERT_EQ (all_responses_equal (responses, 3), true);
         }



         PAXOS_ASSERT_EQ (client.send ("foo").get (), "bar");
         PAXOS_ASSERT_EQ (client.send ("foo").get (), "bar");
         PAXOS_ASSERT_EQ (all_responses_equal (responses, 3), false);
      }

      PAXOS_ASSERT_THROW (client.send ("foo").get (), paxos::exception::no_majority);
   }

   /*!
     Note that we're re-adding servers in reverse order here; this is to ensure that
     server3 doesn't become our leader while it's lagging behind.
    */
   paxos::server server3 ("127.0.0.1", 1339, callback, configuration3);
   server3.add ({{"127.0.0.1", 1337}, {"127.0.0.1", 1338}, {"127.0.0.1", 1339}});
   
   PAXOS_ASSERT_THROW (client.send ("foo").get (), paxos::exception::no_majority);

   paxos::server server2 ("127.0.0.1", 1338, callback, configuration2);
   server2.add ({{"127.0.0.1", 1337}, {"127.0.0.1", 1338}, {"127.0.0.1", 1339}});
   boost::this_thread::sleep (
      boost::posix_time::milliseconds (
         paxos::configuration ().timeout ()));

   PAXOS_ASSERT_EQ (client.send ("foo").get (), "bar");
   PAXOS_ASSERT_EQ (client.send ("foo").get (), "bar");

   paxos::server server1 ("127.0.0.1", 1337, callback, configuration1);
   server1.add ({{"127.0.0.1", 1337}, {"127.0.0.1", 1338}, {"127.0.0.1", 1339}});


   boost::this_thread::sleep (
      boost::posix_time::milliseconds (
         paxos::configuration ().timeout ()));

   do
   {
      PAXOS_ASSERT_EQ (client.send ("foo").get (), "bar");   
      
   } while (all_responses_equal (responses, 3) == false);

   PAXOS_INFO ("test succeeded");
}




