#include <set>
#include <vector>
#include <string>

#include <boost/algorithm/string.hpp>

#include <paxos++/server.hpp>

int main ()
{
   std::set <std::string> locks;

   paxos::server::callback_type callback =
      [& locks](int64_t proposal_id, std::string const & command) -> std::string
      {
         std::vector <std::string> tokens;
         boost::split (tokens, command, boost::is_any_of (" "));

         std::string const & type    = tokens.at (0);
         std::string const & lock_id = tokens.at (1);

         if (type == "lock")
         {
            if (locks.find (lock_id) == locks.end ())
            {
               locks.insert (lock_id);
               return "success";
            }
            else
            {
               return "fail";
            }
         }
         else if (type == "unlock")
         {
            if (locks.find (lock_id) != locks.end ())
            {
               locks.erase (lock_id);
               return "success";
            }
            else
            {
               return "fail";
            }
         }

         // Unrecognized command type
         return "fail";
      };

   paxos::server server ("127.0.0.1", 1337, callback);
   server.add ("127.0.0.1", 1337);

   server.wait ();
}
