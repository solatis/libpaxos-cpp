/*!
  This test validates whether request pipelining inside the client works properly
 */

#include <boost/lexical_cast.hpp>

#include <paxos++/client.hpp>
#include <paxos++/server.hpp>
#include <paxos++/detail/util/debug.hpp>

int main ()
{
   paxos::server server ("127.0.0.1", 1337,
                         [](std::string const & workload) -> std::string
                         {
                            return workload;
                         });
   server.add ("127.0.0.1", 1337);
   server.start ();




   paxos::client client;
   client.add ("127.0.0.1", 1337);
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


