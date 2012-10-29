#include <paxos++/server.hpp>

int main ()
{
   paxos::server server ("127.0.0.1", 1337,
                         [](int64_t proposal_id, std::string const &) -> std::string
                         {
                            return "bar";
                         });
   
   server.add ("127.0.0.1", 1337);
   server.wait ();
}
