/*!
  This test validates whether request pipelining inside the client works properly
 */

#include <boost/lexical_cast.hpp>

#include <paxos++/client.hpp>
#include <paxos++/server.hpp>
#include <paxos++/detail/util/debug.hpp>

int main ()
{
   paxos::server::callback_type callback = 
      [](std::string const & workload) -> std::string
      {
         return workload;
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

   std::map <size_t, std::future <std::string> >     results;
   
   for (size_t i = 0; i < 1000; ++i)
   {
      results[i] = client.send (boost::lexical_cast <std::string> (i), 10);
   }

   
   for (auto & i : results)
   {
      PAXOS_ASSERT (i.second.get () == boost::lexical_cast <std::string> (i.first));
      PAXOS_INFO ("validated result " << i.first);
   }

   PAXOS_INFO ("test succeeded");
}


