#include <set>
#include <vector>
#include <string>

#include <iostream>

#include <boost/algorithm/string.hpp>

#include <paxos++/server.hpp>

#include "datastore.hpp"

int main ()
{
   datastore store;

   paxos::server::callback_type callback =
      [& store](int64_t proposal_id, std::string const & command) -> std::string
      {
         if (proposal_id < store.most_recent_proposal_id ())
         {
            /*! 
              This is a catchup which accidentally re-applies an already applied
              operation.
            */
            std::cerr << "skipping proposal id: " << proposal_id << std::endl;
            return "success";
         }

         std::vector <std::string> tokens;
         boost::split (tokens, command, boost::is_any_of (" "));

         std::string const & type    = tokens.at (0);
         std::string const & lock_id = tokens.at (1);

         if (type == "lock")
         {
            if (store.has_lock (lock_id) == false)
            {
               store.acquire_lock (proposal_id,
                                   lock_id);
               return "success";
            }
            else
            {
               return "fail";
            }
         }
         else if (type == "unlock")
         {
            if (store.has_lock (lock_id) == true)
            {
               store.release_lock (proposal_id,
                                   lock_id);
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
