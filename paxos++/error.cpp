#include "detail/util/debug.hpp"
#include "error.hpp"

namespace paxos {



std::string
to_string (
   enum error_code error_code)
{
   switch (error_code)
   {
         case error_no_leader:
            return "No leader";
            break;

         case error_incorrect_proposal:
            return "Incorrect proposal id";
            break;

         case error_inconsistent_response:
            return "Inconsistent response";
            break;

         case error_connection_timeout:
            return "Connection timeout";
            break;

         case error_connection_close:
            return "Connection closed";
            break;
   };

   PAXOS_UNREACHABLE ();

}


};
