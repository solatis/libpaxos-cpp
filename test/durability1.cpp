/*!
  Tests whether, when a server shuts down from the quorum, is down for a while, and at 
  a later stage it reconnects, it gets properly catched up.
 */

#include <boost/filesystem.hpp>
#include <boost/thread/mutex.hpp>

#include <paxos++/client.hpp>
#include <paxos++/server.hpp>
#include <paxos++/durable/sqlite.hpp>
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

   boost::filesystem::remove ("server1.sqlite");
   boost::filesystem::remove ("server2.sqlite");
   boost::filesystem::remove ("server3.sqlite");

   configuration1.set_durable_storage (
      new paxos::durable::sqlite ("server1.sqlite"));
   configuration2.set_durable_storage (
      new paxos::durable::sqlite ("server2.sqlite"));
   configuration3.set_durable_storage (
      new paxos::durable::sqlite ("server3.sqlite"));


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

   paxos::server server1 ("127.0.0.1", 1337, callback, configuration1);
   paxos::server server2 ("127.0.0.1", 1338, callback, configuration2);

   server1.add ({{"127.0.0.1", 1337}, {"127.0.0.1", 1338}, {"127.0.0.1", 1339}});
   server2.add ({{"127.0.0.1", 1337}, {"127.0.0.1", 1338}, {"127.0.0.1", 1339}});

   paxos::client client;
   client.add  ({{"127.0.0.1", 1337}, {"127.0.0.1", 1338}, {"127.0.0.1", 1339}});

   {
      /*!
        This will be our server that dies.
      */
      paxos::server server3 ("127.0.0.1", 1339, callback, configuration3);

      server3.add ({{"127.0.0.1", 1337}, {"127.0.0.1", 1338}, {"127.0.0.1", 1339}});


      PAXOS_ASSERT_EQ (client.send ("foo").get (), "bar");
      PAXOS_ASSERT_EQ (client.send ("foo").get (), "bar");
      PAXOS_ASSERT_EQ (client.send ("foo").get (), "bar");
      PAXOS_ASSERT_EQ (client.send ("foo").get (), "bar");
      PAXOS_ASSERT_EQ (client.send ("foo").get (), "bar");
      PAXOS_ASSERT_EQ (all_responses_equal (responses, 3), true);
   }

   PAXOS_ASSERT_EQ (client.send ("foo").get (), "bar");
   PAXOS_ASSERT_EQ (client.send ("foo").get (), "bar");
   PAXOS_ASSERT_EQ (client.send ("foo").get (), "bar");
   PAXOS_ASSERT_EQ (client.send ("foo").get (), "bar");
   PAXOS_ASSERT_EQ (client.send ("foo").get (), "bar");
   PAXOS_ASSERT_EQ (all_responses_equal (responses, 3), false);
   

   /*!
     Reconnects our server and waits long enough for connection establishment
     to be retried.
   */
   paxos::server server3 ("127.0.0.1", 1339, callback, configuration3);

   server3.add ({{"127.0.0.1", 1337}, {"127.0.0.1", 1338}, {"127.0.0.1", 1339}});

   boost::this_thread::sleep (
      boost::posix_time::milliseconds (
         paxos::configuration ().timeout ()));

   do
   {
      PAXOS_ASSERT_EQ (client.send ("foo").get (), "bar");   
      
   } while (all_responses_equal (responses, 3) == false);


   boost::filesystem::remove ("server1.sqlite");
   boost::filesystem::remove ("server2.sqlite");
   boost::filesystem::remove ("server3.sqlite");

   PAXOS_INFO ("test succeeded");
}




