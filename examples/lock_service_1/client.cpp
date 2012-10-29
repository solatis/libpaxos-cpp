#include <paxos++/client.hpp>

int main ()
{
   paxos::client client;
   client.add ("127.0.0.1", 1337);

   assert (client.send ("lock foo").get () == "success");
   assert (client.send ("lock foo").get () == "fail");

   assert (client.send ("lock bar").get () == "success");
   assert (client.send ("lock bar").get () == "fail");

   assert (client.send ("unlock foo").get () == "success");
   assert (client.send ("unlock bar").get () == "success");

   assert (client.send ("unlock foo").get () == "fail");
   assert (client.send ("unlock bar").get () == "fail");
}
