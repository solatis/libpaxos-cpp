#include <paxos++/client.hpp>

int main ()
{
   paxos::client client;
   client.add ("127.0.0.1", 1337);

   std::future <std::string> future = client.send ("foo");
   assert (future.get () == "bar");
}
